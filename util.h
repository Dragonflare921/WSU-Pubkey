/////////////////////////////////////////////////////////////////////
// utils
// 
// common utility functions and defines
//
// Brandon Warner
// @dragonflare921
// dragonflare921@gmail.com
//
//

// header guard
#ifndef _UTIL_H_
#define _UTIL_H_


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



//////////////////////////////////////////
// DEFINES

// BOOLEAN VALUES
#define true    0x01
#define false   0x00

// MASKS

// basic bit position
#define BIT0    0x01
#define BIT1    0x02
#define BIT2    0x04
#define BIT3    0x08
#define BIT4    0x10
#define BIT5    0x20
#define BIT6    0x40
#define BIT7    0x80

// message type name
#define P_SILENT    BIT0
#define P_ERROR     BIT1
#define P_WARN      BIT2
#define P_INFO      BIT3
#define P_DEBUG     BIT4
#define P_ASSERT    BIT5
// TODO (Dragon): think about these
#define P_EVENT     BIT6
#define P_ALERT     BIT7

// MAXIMUMS
#define BLOCK_SIZE  8   // TODO (Dragon): probably rename this now
#define TAG_SIZE    4   // size of message tags
#define BUFF_SIZE   512 // maximum buffer size to copy
#define MAX_PATH    BUFF_SIZE // maximum path length


//////////////////////////////////////////
// MACROS

// BITWISE OPS
// providing both shift and mask options for completeness
// TODO (Dragon): make these work for both the defined address volatiles, and regular lvalues
//       maybe bring the cast into this macro instead of the define?
//       then the define becomes just the address, and we can use a variable's
//       address with the & operator when doing it on normal variables

// test bits
#define TEST_BIT_MASK(v, m) ((v & m)?true:false)
#define TEST_BIT_SHIFT(v, s) ((v & (0x01 << s))?true:false)

// set bits
#define SET_BIT_MASK(v, m) (v |= m)
#define SET_BIT_SHIFT(v, s) (v |= (0x01 << s))

// clear bits
#define CLEAR_BIT_MASK(v, m) (v &= ~(m))
#define CLEAR_BIT_SHIFT(v, m) (v &= ~(0x01 << s))



//////////////////////////////////////////
// TYPEDEFS

// ATOMICS

// unsigned
typedef uint8_t     U8;     // unsigned 8 bit integer value
typedef uint16_t    U16;    // unsigned 16 bit integer value
typedef uint32_t    U32;    // unsigned 32 bit integer value
typedef uint64_t    U64;    // unsigned 64 bit integer value
// signed
typedef int8_t      I8;     // signed 8 bit integer value
typedef int16_t     I16;    // signed 16 bit integer value
typedef int32_t     I32;    // signed 32 bit integer value
typedef int64_t     I64;    // signed 64 bit integer value
// floating point
typedef float       F32;    // signed 32 bit floating point value
typedef double      F64;    // signed 64 bit floating point value
// convenient names
typedef U8          BYTE;   // sensible name for a byte
// TODO (Dragon): flags/bools should have a way to pack into bitfields at compile time
// boolean
typedef BYTE        BOOL;   // sensible name for a boolean
typedef BOOL        FLAG;   // alternate name for boolean
// bitfields
typedef U8          BF8;    // 8 bit wide bitfield
typedef U16         BF16;   // 16 bit wide bitfield
typedef U32         BF32;   // 32 bit wide bitfield
typedef U64         BF64;   // 64 bit wide bitfield
// padding
typedef U8          PAD8;   // 8 bit wide padding
typedef U16         PAD16;  // 16 bit wide padding
typedef U32         PAD32;  // 32 bit wide padding
typedef U64         PAD64;  // 64 bit wide padding
// TODO (Dragon): should we do this for strings?
// strings
typedef const char* C_STR;  // constant string
typedef char*       STR;    // variable string
// TODO (Dragon): is this goint to make sense lol?
typedef char        PATH[MAX_PATH];


// ENUMS

// print message type
typedef enum MSG_TYPE {
    INFO_MSG,
    WARN_MSG,
    ERROR_MSG,
    DEBUG_MSG,
    ASSERT_MSG,
    // TODO (Dragon): think about use cases
    EVENT_MSG,
    ALERT_MSG,
    
    // leave this last
    UNKNOWN_MSG
} MSG_TYPE;

// error types
typedef enum ERR_TYPE {
    ERR_OK,
    ERR_OPEN,
    ERR_CLOSE,
    ERR_READ,
    ERR_WRITE,
    ERR_SIZE,
    ERR_BUFFER,
    ERR_CHAR,
    ERR_ARGS,
    ERR_MODE,
    ERR_VERBOSITY,
    
    
    // leave this last
    ERR_UNK
} ERR_TYPE;




// TODO (Dragon): convert these to macros

//////////////////////////////////////////
// BYTE ORDER CONVERSION

U16 bswap16(U16 in);  // swap 16 bit byte order
U32 bswap32(U32 in);  // swap 32 bit byte order
U64 bswap64(U64 in);  // swap 64 bit byte order

// rotates
void rrotate(BYTE* array, U32 size, U32 shift);
void lrotate(BYTE* array, U32 size, U32 shift);

// concatenators
U16 catbytes(BYTE b1, BYTE b2);



//////////////////////////////////////////
// BUFFER CONVERSION
// NOTE: size param represents the byte count for the *BYTE BUFFER*
//       regardless of the conversion, use the number of raw bytes
//       DO NOT PASS THE STRING BUFFER LENGTH

// convert from a hex string buffer of length 2*size to a byte buffer of length size
ERR_TYPE hexstr_bytes(C_STR strbuff, BYTE* bytebuff, U32 size);

// convert from a byte buffer of length size to a hex string buffer of length 2*size
ERR_TYPE bytes_hexstr(BYTE* bytebuff, STR strbuff, U32 size);



//////////////////////////////////////////
// GLOBALS

#ifdef _PRETTY_PRINTER_
C_STR INFO_MSG_TAG = "INFO";            // info message tag
C_STR WARN_MSG_TAG = "WARN";            // warning message tag
C_STR ERROR_MSG_TAG = "ERR!";           // error message tag
C_STR DEBUG_MSG_TAG = "DBUG";           // debug message tag
C_STR ASSERT_MSG_TAG = "ASRT";          // assert message tag
C_STR UNKNOWN_MSG_TAG = "?UNK";         // unknown message tag
// TODO (Dragon): think about use cases
C_STR EVENT_MSG_TAG = "EVNT";           // event message tag
C_STR ALERT_MSG_TAG = "ALRT";           // alert message tag
//#endif //_PRETTY_PRINTER_
#else
#define INFO_MSG_TAG  "INFO\0"            // info message tag
#define WARN_MSG_TAG  "WARN\0"            // warning message tag
#define ERROR_MSG_TAG  "ERR!\0"           // error message tag
#define DEBUG_MSG_TAG  "DBUG\0"           // debug message tag
#define ASSERT_MSG_TAG  "ASRT\0"          // assert message tag
#define UNKNOWN_MSG_TAG  "?UNK\0"         // unknown message tag
// TODO (Dragon): think about use cases
#define EVENT_MSG_TAG  "EVNT\0"          // event message tag
#define ALERT_MSG_TAG  "ALRT\0"           // alert message tag
#endif


//////////////////////////////////////////
// PRINTERS

void print_pretty(MSG_TYPE type, C_STR loc, C_STR msg, BF8 mask); // custom pretty printer



//////////////////////////////////////////
// HASHING

// djb2 hash function written by Dan Bernstein
// http://www.cse.yorku.ca/~oz/hash.html
U32 djb2_hash(STR in);


//////////////////////////////////////////
// RANDOM NUMBER GENERATION
U32 rand32();   // generate a random 32 bit number

#endif //_UTIL_H_
