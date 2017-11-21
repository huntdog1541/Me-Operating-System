#include "utility.h"

char hexes[] = "0123456789ABCDEF";
char alphabet[] = "abcdefghijklmnopqrstuvwxyz";



void uitoa(uint32 val, char* buffer, uint8 base)
{
	uint16 i = 0;
	do
	{
		buffer[i] = hexes[val % base];
		i++;
	} while ((val /= base) != 0);

	buffer[i] = 0;

	//reverse

	uint16 length = 0;
	while (buffer[length] != '\0')
		length++;
	i = 0;
	for (; i < length / 2; i++)
	{
		char temp = buffer[i];
		buffer[i] = buffer[length - 1 - i];
		buffer[length - 1 - i] = temp;
	}

	buffer[length] = 0;		// not required but to be sure. buffer[i] = 0 above is enough and is not affected by the reverse process.
}

uint32 ceil_division(uint32 value, uint32 divisor)
{
	uint32 div = value / divisor;
	uint32 rem = value % divisor;

	if (rem != 0)
		div++;

	return div;
}

uint32 atoui(char* input)
{
	uint16 length = 0;
	while (input[length] != '\0')
		length++;

	if (input[0] == '0' && tolower(input[1]) == 'x')	// hex
		return atoui_hex(input + 2, length - 2);
	else
		return atoui_dec(input, length);
}

uint32 atoui_hex(char* input, uint16 length)
{
	uint32 res = 0;

	for (uint32 i = 0; i < length; i++)
	{
		uint32 mult = input[i] - '0';

		if (isalpha(input[i]))
			mult = 10 + tolower(input[i]) - 'a';	// convert

		res += mult * pow(16, length - 1 - i);
	}

	return res;
}

uint32 pow(uint32 base, uint32 exp)	// IMPROVEEEEE
{
	uint32 res = 1;;
	for (uint32 i = 0; i < exp; i++)
		res *= base;
	return res;
}

uint32 atoui_dec(char* input, uint16 length)
{
	uint32 res = 0;

	for (uint32 i = 0; i < length; i++)
		res += (input[i] - '0') * pow(10, length - 1 - i);

	return res;
}

void printf_base(char* fmt, va_list arg_start)
{
	uint16 length = 0;
	while (fmt[length] != '\0')
		length++;

	uint32 val;
	uint8 cval;
	uint64 lval;
	char* ptr;
	char buffer[20];

	for (uint16 i = 0; i < length; i++)
	{
		switch (fmt[i])
		{
		case '%':

			switch (fmt[i + 1])
			{
			case 'u':
				val = va_arg(arg_start, uint32);
				uitoa(val, buffer, 10);
				Print(buffer);
				break;
			case 'h':
				val = va_arg(arg_start, uint32);
				uitoa(val, buffer, 16);
				Print("0x");
				Print(buffer);
				break;
			case 'x':
				val = va_arg(arg_start, uint32);
				uitoa(val, buffer, 16);
				Print(buffer);
				break;
			case 's':
				ptr = va_arg(arg_start, char*);
				Print(ptr);
				break;
			case 'c':
				cval = va_arg(arg_start, uint32);
				Printch(cval);
				break;
			case 'l':
				lval = va_arg(arg_start, uint64);
				uitoa(lval, buffer, 10);
				Print(buffer);
				break;
			case 'b':
				val = va_arg(arg_start, uint32);
				uitoa(val, buffer, 2);
				Print(buffer);
			default:
				break;
			}

			i++;
			break;

		default:
			Printch(fmt[i]); break;
		}
	}
}

void printf(char* fmt, ...)
{
	va_list l;
	va_start(l, fmt);	// spooky stack magic going on here

	printf_base(fmt, l);

	va_end(l);
}

void printfln(char* fmt, ...)
{
	va_list l;
	va_start(l, fmt);	// spooky stack magic going on here

	printf_base(fmt, l);

	va_end(l);

	Printch('\n');
}

void memset(void* base, uint8 val, uint32 length)
{
	char* ptr = (char*)base;

	uint32 index = length % sizeof(uint32);

	for (uint32 j = 0; j < index; j++)
	{
		*ptr = val;
		ptr++;
		length--;
	}

	uint32* new_ptr = (uint32*)ptr;
	length /= sizeof(uint32);

	//create the 32 bit value so that the last loop works
	uint32 val32 = val & 0xFF;
	val32 |= (val << 8) & 0xFF00;
	val32 |= (val << 16) & 0xFF0000;
	val32 |= (val << 24) & 0xFF000000;

	for (uint32 i = 0; i < length; i++)	// must be strictly less than length
		new_ptr[i] = val32;
}

void PANIC(char* str)
{
	SetColor(MakeColor(DARK_BLUE, WHITE));
	//ClearScreen();

	SetCursor(0, 13);
	printf("L O A D E R  E X C E P T I O N\n%s", str);

	__asm
	{
		cli
		hlt
	}
}