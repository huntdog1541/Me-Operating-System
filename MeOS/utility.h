#ifndef UTILITY_H
#define UTILITY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "screen.h"
#include "cstring.h"

#define isdigit(arg) ( arg >= '0' && arg <= '9' ? 1 : 0 )
#define isalpha(arg) ( ((arg >= 'a' && arg <= 'z') || (arg >= 'A' && arg <= 'Z')) ? 1 : 0 )
#define isalnum(arg) ( (isdigit(arg) || isalpha(arg)) ? 1 : 0 )
#define isupper(arg) ( (arg >= 'A' && arg <= 'Z') ? 1 : 0 )
#define islower(arg) ( (arg >= 'a' && arg <= 'z') ? 1 : 0 )

#define isprint(arg) ( arg >= 0x1f && arg < 0x7f )

#define tolower(arg) (  isupper(arg) ? arg + 32 : arg )
#define toupper(arg) (  islower(arg) ? arg - 32 : arg )

#define min(a, b)	 ( a < b ? a : b )
#define max(a, b)	 ( a < b ? b : a )

	extern char hexes[];
	extern char alphabet[];

	// takes an unsigned int and returns to buffer its string base-representation
	void uitoa(uint32 val, char* buffer, uint8 base);
	void itoa(int32 val, char* buffer, uint8 base);

	// takes an unsigned int and returns its alpha representation
	// 1 -> a | 2 -> b | 25 -> aa
	void uitoalpha(uint32 val, char* buffer);

	void memset(void* base, uint8 val, uint32 length);
	void memcpy(void* dest, void* source, uint32 length);

	uint32 atoui(char* buffer);
	uint32 atoui_dec(char* buffer, uint16 length);
	uint32 atoui_hex(char* buffer, uint16 length);

	uint32 pow(uint32 base, uint32 exp);
	uint32 ceil_division(uint32 value, uint32 divisor);

	// returns the netowrk representation of the long host parameter
	uint32 htonl(uint32 host);

	// returns the network representation of the short host parameter
	uint16 htons(uint16 host);

	// returns the host representation of the long net parameter
	uint32 ntohl(uint32 net);

	// returns the host representation of the short net parameter
	uint16 ntohs(uint16 net);

#ifdef __cplusplus
}
#endif

#endif