#include "descriptor_tables.h"

gdt_entry_t*	gdt_entries;
gdt_ptr_t 		gdt_ptr;

idt_entry_t*	idt_entries;
idt_ptr_t 		idt_ptr;

void DivisionByZero(registers_t* regs)
{
	PANIC("Divion by zero");
}

void GPF(registers_t* regs)
{
	char str[40] = "General Protection Fault at: ";
	uitoa(regs->eip, str + 28, 16);
	PANIC(str);
}

void DebugException(registers_t* regs)
{
	PANIC("Debug Exception");
}

void OverflowException(registers_t* regs)
{
	PANIC(__FUNCTION__);
}

void BRE(registers_t* regs)
{
	PANIC(__FUNCTION__);
}

void InvalidOp(registers_t* regs)
{
	char str[30] = "Invalid Opcode at: ";
	uitoa(regs->eip, str + 19, 16);
	PANIC(str);
}

void DevNotAvail(registers_t* regs)
{
	PANIC(__FUNCTION__);
}

void DoubleFault(registers_t* regs)
{
	PANIC(__FUNCTION__);
}

void InvalidTSS(registers_t* regs)
{
	PANIC(__FUNCTION__);
}

void SegmentNotPresent(registers_t* regs)
{
	PANIC(__FUNCTION__);
}

void StackSegError(registers_t* regs)
{
	PANIC(__FUNCTION__);
}

void PageFault(registers_t* regs)
{
	uint32 addr;
	_asm
	{
		mov eax, cr2
		mov dword ptr addr, eax
	}

	printfln("PAGE_FALUT: FAULTING ADDRESS: %h", addr);
	PANIC(__FUNCTION__);
}

void FloatingPtException(registers_t* regs)
{
	PANIC(__FUNCTION__);
}

void TripleFault(registers_t* regs)
{
	PANIC(__FUNCTION__);
}

void init_descriptor_tables()
{
	init_gdt();
	init_idt();

	memset(interrupt_handlers, 0, sizeof(isr_t) * ISR_HANDLERS);

	// regsiter the basic set of exceptions
	register_interrupt_handler(0, DivisionByZero);
	register_interrupt_handler(1, DebugException);
	register_interrupt_handler(4, OverflowException);
	register_interrupt_handler(5, BRE);
	register_interrupt_handler(6, InvalidOp);
	register_interrupt_handler(7, DevNotAvail);
	register_interrupt_handler(8, DoubleFault);
	register_interrupt_handler(10, InvalidTSS);
	register_interrupt_handler(11, SegmentNotPresent);
	register_interrupt_handler(12, StackSegError);
	register_interrupt_handler(13, GPF);
	register_interrupt_handler(14, PageFault);
}

void init_gdt()
{
	gdt_entries = kalloc(GDT_ENTRIES * sizeof(gdt_entry_t));

	gdt_ptr.base = (uint32)gdt_entries;
	gdt_ptr.limit = GDT_ENTRIES * sizeof(gdt_entry_t) - 1;

	gdt_set_gate(0, 0, 0, 0, 0);				// null dummy segment
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);	// Code segment
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);	// Data segment
	gdt_set_gate(3, 0, 0xFFFFF, 0xFA, 0xCF);	// User mode code segment
	gdt_set_gate(4, 0, 0xFFFFF, 0xF2, 0xCF);	// User mode data segment

	flush_gdt((uint32)&gdt_ptr);
}

void init_idt()
{
	idt_entries = kalloc(ISR_HANDLERS * sizeof(idt_entry_t));

	idt_ptr.base = (uint32)idt_entries;
	idt_ptr.limit = ISR_HANDLERS * sizeof(idt_entry_t) - 1;

	memset(idt_entries, 0x0, 256 * sizeof(idt_entry_t));

	idt_set_gate(0, (uint32)isr0, 0x08, 0x8E);
	idt_set_gate(1, (uint32)isr1, 0x08, 0x8E);
	idt_set_gate(2, (uint32)isr2, 0x08, 0x8E);
	idt_set_gate(3, (uint32)isr3, 0x08, 0x8E);
	idt_set_gate(4, (uint32)isr4, 0x08, 0x8E);
	idt_set_gate(5, (uint32)isr5, 0x08, 0x8E);
	idt_set_gate(6, (uint32)isr6, 0x08, 0x8E);
	idt_set_gate(7, (uint32)isr7, 0x08, 0x8E);
	idt_set_gate(8, (uint32)isr8, 0x08, 0x8E);
	idt_set_gate(9, (uint32)isr9, 0x08, 0x8E);
	idt_set_gate(10, (uint32)isr10, 0x08, 0x8E);
	idt_set_gate(11, (uint32)isr11, 0x08, 0x8E);
	idt_set_gate(12, (uint32)isr12, 0x08, 0x8E);
	idt_set_gate(13, (uint32)isr13, 0x08, 0x8E);
	idt_set_gate(14, (uint32)isr14, 0x08, 0x8E);
	idt_set_gate(15, (uint32)isr15, 0x08, 0x8E);
	idt_set_gate(16, (uint32)isr16, 0x08, 0x8E);
	idt_set_gate(17, (uint32)isr17, 0x08, 0x8E);
	idt_set_gate(18, (uint32)isr18, 0x08, 0x8E);
	idt_set_gate(19, (uint32)isr19, 0x08, 0x8E);
	idt_set_gate(20, (uint32)isr20, 0x08, 0x8E);
	idt_set_gate(21, (uint32)isr21, 0x08, 0x8E);
	idt_set_gate(22, (uint32)isr22, 0x08, 0x8E);
	idt_set_gate(23, (uint32)isr23, 0x08, 0x8E);
	idt_set_gate(24, (uint32)isr24, 0x08, 0x8E);
	idt_set_gate(25, (uint32)isr25, 0x08, 0x8E);
	idt_set_gate(26, (uint32)isr26, 0x08, 0x8E);
	idt_set_gate(27, (uint32)isr27, 0x08, 0x8E);
	idt_set_gate(28, (uint32)isr28, 0x08, 0x8E);
	idt_set_gate(29, (uint32)isr29, 0x08, 0x8E);
	idt_set_gate(30, (uint32)isr30, 0x08, 0x8E);
	idt_set_gate(31, (uint32)isr31, 0x08, 0x8E);

	idt_set_gate(32, (uint32)irq0, 0x08, 0x8E);
	idt_set_gate(33, (uint32)irq1, 0x08, 0x8E);
	idt_set_gate(34, (uint32)irq2, 0x08, 0x8E);
	idt_set_gate(35, (uint32)irq3, 0x08, 0x8E);
	idt_set_gate(36, (uint32)irq4, 0x08, 0x8E);
	idt_set_gate(37, (uint32)irq5, 0x08, 0x8E);
	idt_set_gate(38, (uint32)irq6, 0x08, 0x8E);
	idt_set_gate(39, (uint32)irq7, 0x08, 0x8E);
	idt_set_gate(40, (uint32)irq8, 0x08, 0x8E);
	idt_set_gate(41, (uint32)irq9, 0x08, 0x8E);
	idt_set_gate(42, (uint32)irq10, 0x08, 0x8E);
	idt_set_gate(43, (uint32)irq11, 0x08, 0x8E);
	idt_set_gate(44, (uint32)irq12, 0x08, 0x8E);
	idt_set_gate(45, (uint32)irq13, 0x08, 0x8E);
	idt_set_gate(46, (uint32)irq14, 0x08, 0x8E);
	idt_set_gate(47, (uint32)irq15, 0x08, 0x8E);

	flush_idt((uint32)&idt_ptr);
}

void gdt_set_gate(uint16 num, uint32 base, uint32 limit, uint8 access, uint8 gran)
{
	gdt_entries[num].base_low = base & 0x0000FFFF;
	gdt_entries[num].base_middle = (base >> 16) & 0x000000FF;
	gdt_entries[num].base_high = (base >> 24) & 0x000000FF;

	gdt_entries[num].limit_low = limit & 0xFFFF;
	gdt_entries[num].granularity = (limit >> 16) & 0x0F;	// first 4 bits of granularity go to limit_"high"

	gdt_entries[num].granularity |= gran & 0xF0;
	gdt_entries[num].access = access;
}

void idt_set_gate(uint8 num, uint32 base, uint16 selector, uint8 flags)
{
	idt_entries[num].base_low = base & 0x0000FFFF;
	idt_entries[num].base_high = (base >> 16) & 0xFFFF;	// and with FFFF for safety. >> operator fills 0 in

	idt_entries[num].selector = selector;
	idt_entries[num].always0 = 0;
	idt_entries[num].flags = flags;
}