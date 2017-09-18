#include "i217.h"
#include "print_utility.h"
#include "SerialDebugger.h"
#include "isr.h"
#include "ethernet.h"
#include "arp.h"
#include "mmngr_virtual.h"

void e1000_write_command(e1000* dev, uint16 addr, uint32 value)
{
	if (dev->bar_type == 0)
		*(uint32*)(dev->mem_base + addr) = value;
	else
	{
		outportl(dev->io_base, addr);
		outportl(dev->io_base + 4, value);
	}
}

uint32 e1000_read_command(e1000* dev, uint16 addr)
{
	if (dev->bar_type == 0)
		return *(uint32*)(dev->mem_base + addr);
	else
	{
		outportl(dev->io_base, addr);
		return inportl(dev->io_base + 4);
	}
}

bool e1000_detect_eeprom(e1000* dev)
{
	uint32 val = 0;
	e1000_write_command(dev, REG_EEPROM, 0x1);

	dev->eeprom_exists = false;

	for (int i = 0; i < 1000 & !dev->eeprom_exists; i++)
	{
		val = e1000_read_command(dev, REG_EEPROM);

		if (val & 0x10)
			dev->eeprom_exists = true;
	}

	return dev->eeprom_exists;
}

uint32 e1000_eeprom_read(e1000* dev, uint8 addr)
{
	uint16 data = 0;
	uint32 temp = 0;

	if (dev->eeprom_exists)
	{
		e1000_write_command(dev, REG_EEPROM, 1 | (((uint32)addr) << 8));
		while (!((temp = e1000_read_command(dev, REG_EEPROM)) & (1 << 4)));
	}
	else
	{
		e1000_write_command(dev, REG_EEPROM, 1 | (((uint32)addr) << 2));
		while (!((temp = e1000_read_command(dev, REG_EEPROM)) & (1 << 1)));
	}

	data = (uint16)((temp >> 16) & 0xFFFF);
	return data;
}

bool e1000_read_mac_address(e1000* dev)
{
	if (dev->eeprom_exists)
	{
		uint32 temp;
		temp = e1000_eeprom_read(dev, 0);
		dev->mac[0] = temp & 0xff;
		dev->mac[1] = temp >> 8;
		temp = e1000_eeprom_read(dev, 1);
		dev->mac[2] = temp & 0xff;
		dev->mac[3] = temp >> 8;
		temp = e1000_eeprom_read(dev, 2);
		dev->mac[4] = temp & 0xff;
		dev->mac[5] = temp >> 8;
	}
	else
	{
		uint8* mem_base_mac_8 = (uint8 *)(dev->mem_base + 0x5400);
		uint32* mem_base_mac_32 = (uint32 *)(dev->mem_base + 0x5400);

		if (mem_base_mac_32[0] != 0)
			for (int i = 0; i < 6; i++)
				dev->mac[i] = mem_base_mac_8[i];
		else return false;
	}

	return true;
}

void rx_init(e1000* dev, physical_addr base_rx)
{
	e1000_rx_desc* descs = (e1000_rx_desc*)base_rx;

	for (uint32 i = 0; i < E1000_NUM_RX_DESC; i++)
	{
		dev->rx_descs[i] = (e1000_rx_desc*)(descs + i);
		dev->rx_descs[i]->addr = 0x360000 + i * (2048 + 16);
		dev->rx_descs[i]->status = 0;
	}

	e1000_write_command(dev, REG_RXDESCLO, base_rx);
	e1000_write_command(dev, REG_RXDESCHI, 0);

	e1000_write_command(dev, REG_RXDESCLEN, E1000_NUM_RX_DESC * 16);

	e1000_write_command(dev, REG_RXDESCHEAD, 0);
	e1000_write_command(dev, REG_RXDESCTAIL, E1000_NUM_RX_DESC - 1);
	dev->rx_cur = 0;
	e1000_write_command(dev, REG_RCTRL, RCTL_EN | RCTL_SBP | RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC | RCTL_BSIZE_2048);
}

void tx_init(e1000* dev, physical_addr base_tx)
{
	e1000_rx_desc* descs = (e1000_rx_desc*)base_tx;

	for (uint32 i = 0; i < E1000_NUM_TX_DESC; i++)
	{
		dev->tx_descs[i] = (e1000_tx_desc*)((uint8*)descs + i * sizeof(e1000_tx_desc));
		dev->tx_descs[i]->addr = 0;
		dev->tx_descs[i]->cmd = 0;
	}

	e1000_write_command(dev, REG_TXDESCLO, base_tx);
	e1000_write_command(dev, REG_TXDESCHI, 0);
	e1000_write_command(dev, REG_TXDESCLEN, E1000_NUM_TX_DESC * sizeof(e1000_tx_desc));

	e1000_write_command(dev, REG_TXDESCHEAD, 0);
	e1000_write_command(dev, REG_TXDESCTAIL, E1000_NUM_TX_DESC);
	dev->tx_cur = 0;
	e1000_write_command(dev, REG_TCTRL, TCTL_EN
		| TCTL_PSP
		| (15 << TCTL_CT_SHIFT)
		| (64 << TCTL_COLD_SHIFT)
		| TCTL_RTLC);

	e1000_write_command(dev, REG_TCTRL, 0b0110000000000111111000011111010);
	e1000_write_command(dev, REG_TIPG, 0x0060200A);
}

int e1000_sendPacket(e1000* dev, void* data, uint16 p_len)
{
	dev->tx_descs[dev->tx_cur]->addr = vmmngr_get_phys_addr((virtual_addr)data);
	dev->tx_descs[dev->tx_cur]->length = p_len;
	dev->tx_descs[dev->tx_cur]->cmd = /*((1 << 3) | (3));*/ CMD_EOP | CMD_IFCS | CMD_RS | CMD_RPS;
	dev->tx_descs[dev->tx_cur]->status = 0;
	uint8 old_cur = dev->tx_cur;
	dev->tx_cur = (dev->tx_cur + 1) % E1000_NUM_TX_DESC;
	e1000_write_command(dev, REG_TXDESCTAIL, dev->tx_cur);

	while (!(dev->tx_descs[old_cur]->status & 0xf));
	serial_printf("packet sent\n");
	return 0;
}

uint32 recv_packet = 0;

void e1000_recv_packet(e1000* dev)
{
	uint8 *pkt = (uint8 *)dev->rx_descs[dev->rx_cur]->addr;
	uint16 pktlen = dev->rx_descs[dev->rx_cur]->length;

	dev->rx_cur = (dev->rx_cur + 1) % E1000_NUM_RX_DESC;

	// write the tail to the device
	e1000_write_command(dev, REG_RXDESCTAIL, dev->rx_cur);

	eth_header* eth = (eth_header*)pkt;

	//serial_printf("received packets: %u length: %u address: %h\n", recv_packet++, pktlen, pkt);

	if (ntohs(eth->eth_type) == 0x0806 && (eth_cmp_mac(eth->dest_mac, dev->mac) || eth_cmp_mac(eth->dest_mac, mac_broadcast)))
	{
		arp_ipv4* arp = (arp_ipv4*)((arp_header*)eth->eth_data)->data;

		arp_receive((arp_header*)eth->eth_data);

		//eth_print(eth);
		serial_printf("arp source mac: %x %x %x %x %x %x\n", arp->src_mac[0], arp->src_mac[1], arp->src_mac[2], arp->src_mac[3], arp->src_mac[4], arp->src_mac[5]);
		serial_printf("arp source ip: %u.%u.%u.%u\n", arp->src_ip[0], arp->src_ip[1], arp->src_ip[2], arp->src_ip[3]);
		serial_printf("arp destination ip: %u.%u.%u.%u----\n", arp->dest_ip[0], arp->dest_ip[1], arp->dest_ip[2], arp->dest_ip[3]);
	}

}

void e1000_callback(registers_t* regs)
{
	extern e1000* nic_dev;

	uint32 icr = e1000_read_command(nic_dev, 0xC0);

	if (icr & 0x4)
		printfln("link status changed");

	if (icr & 0x80)
		e1000_recv_packet(nic_dev);
}


void e1000_enable_interrupts(e1000* dev)
{
	e1000_write_command(dev, REG_IMASK, 0x1F6DC);
	e1000_write_command(dev, REG_IMASK, 0xff & ~4);
	e1000_read_command(dev, 0xc0);

	register_interrupt_handler(32 + 10, e1000_callback);
}


e1000* e1000_start(uint8 bar_type, uint32 mem_base, physical_addr tx_base, physical_addr rx_base)
{
	e1000* dev = new e1000;
	dev->bar_type = bar_type;
	dev->mem_base = mem_base;

	serial_printf("--------eeprom detection\n");
	e1000_detect_eeprom(dev);
	serial_printf("--------read MAC\n");
	e1000_read_mac_address(dev);

	// setup link state
	e1000_write_command(dev, REG_CTRL, (e1000_read_command(dev, REG_CTRL) | CTRL_SLU));

	for (int i = 0; i < 0x80; i++)
		e1000_write_command(dev, 0x5200 + i * 4, 0);

	serial_printf("MAC address: %x %x %x %x %x %x\n", dev->mac[0], dev->mac[1], dev->mac[2], dev->mac[3], dev->mac[4], dev->mac[5]);

	e1000_enable_interrupts(dev);

	serial_printf("--------initialization of tx\n");
	rx_init(dev, 0x350000);
	tx_init(dev, tx_base);
	serial_printf("--------end initialization of tx\n");

	return dev;
}