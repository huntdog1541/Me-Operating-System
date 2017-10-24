#ifndef TYPES_H
#define TYPES_H

#define bool uint8
#define true 1
#define false 0

typedef signed char int8;
typedef unsigned char uint8;

typedef signed short int16;
typedef unsigned short uint16;

typedef signed int int32;
typedef unsigned int uint32;

typedef signed long long int64;
typedef unsigned long long uint64;

typedef uint8 BYTE;
typedef uint16 WORD;
typedef uint32 DWORD;
typedef uint64 QWORD;
typedef void   VOID;

#define KB *1024
#define MB KB*1024
#define GB MB*1024

#define PTR *

typedef uint8* va_list;

#define sz(arg_type) ( sizeof(arg_type) % 4 != 0 ? sizeof(arg_type) + 4 - sizeof(arg_type) % 4 : sizeof(arg_type) )

#define va_start(pointer, last_arg) (  pointer = ( (va_list)&last_arg + sz(last_arg) )  )
#define va_end(pointer) (  pointer = (va_list) 0x0  )
#define va_arg(pointer, arg_type) ( *(arg_type*)( (pointer += sz(arg_type)) - sz(arg_type) ) )

#endif