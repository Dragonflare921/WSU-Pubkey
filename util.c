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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>

// fix some
#define _PRETTY_PRINTER_
#include "util.h"
#undef _PRETTY_PRINTER_


//////////////////////////////////////////
// HASHING

// djb2 hash function written by Dan Bernstein
// http://www.cse.yorku.ca/~oz/hash.html
// slightly modified to conform to util.h types
U32 djb2_hash(STR in) {
  U32 hash = 5381;
  I32 c;

  while ((c = (*in++))) {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }
  

  return hash;
}


//////////////////////////////////////////
// BYTE ORDERING

// swap 16 bit byte order
U16 bswap16(U16 in) {
  U16 ret;
  ret  = (in & 0x00FF) << 8;
  ret |= (in & 0xFF00) >> 8;
  return ret;
}

// swap 32 bit byte order
U32 bswap32(U32 in) {
  U32 ret;
  ret  = (in & 0x000000FF) << 24;
  ret |= (in & 0x0000FF00) << 8;
  ret |= (in & 0x00FF0000) >> 8;
  ret |= (in & 0xFF000000) >> 24;
  return ret;
}

// swap 64 bit byte order
U64 bswap64(U64 in) {
  U64 ret;
  ret  = (in & 0x00000000000000FF) << 56;
  ret |= (in & 0x000000000000FF00) << 40;
  ret |= (in & 0x0000000000FF0000) << 24;
  ret |= (in & 0x00000000FF000000) << 8;
  ret  = (in & 0x000000FF00000000) >> 8;
  ret |= (in & 0x0000FF0000000000) >> 24;
  ret |= (in & 0x00FF000000000000) >> 40;
  ret |= (in & 0xFF00000000000000) >> 56;
  return ret;
}

// bitwise rotate a byte array right
void rrotate(BYTE* array, U32 size, U32 shift) {

  // store the original size
  U32 osize = size;
  
  // outer loop to do rotates by more than 1
  for (I32 i = 0; i < shift; i++) {
    
    // restore the original size
    size = osize;
    
    // carry bits from shifting
    BYTE thisbit = 0;
    BYTE nextbit = thisbit;
    
    // shift each byte and bring the carry forward
    for (I32 j = 0; j < size; j++) {
      thisbit = nextbit;
      nextbit = (array[j] & 1) << 7;
      array[j] >>= 1;
      array[j] |= thisbit;
    }
    
    // finish rotation
    array[0] |= nextbit;
  }
  
  return;
}

// bitwise rotate a byte array left
void lrotate(BYTE* array, U32 size, U32 shift) {
  
  // store the original size
  U32 osize = size;
  
  // outer loop to do rotates by more than 1
  for (I32 i = 0; i < shift; i++) {
    
    // restore the original size
    size = osize;
    
    // carry bits from shifting
    BYTE thisbit = 0;
    BYTE nextbit = thisbit;
    
    // shift each byte and bring the carry forward
    for (I32 j = size-1; j >= 0; j--) {
      thisbit = nextbit;
      nextbit = (array[j] & 0x80) >> 7;
      array[j] <<= 1;
      array[j] |= thisbit;
    }
    
    // finish rotation
    array[osize-1] |= nextbit;
  }
  
  return;
}

// return a 16bit short resulting from the concatenation of 2 bytes
U16 catbytes(BYTE b1, BYTE b2) {
  return ((b1 << 8) | b2);
}


// byte array <---> hex string conversions
// NOTE: tried to make these as small and fast as possible for later reuse
//       since i hadn't writen these *very useful* util functions in C before
//       it's important to note these treat things as a raw byte array
//       theres no endianness or byte swapping or division into fields at all
//       the order it comes in from the string is the order it goes out

// convert from a hex string buffer of length 2*size to a byte buffer of length size
ERR_TYPE hexstr_bytes(C_STR strbuff, BYTE* bytebuff, U32 size) {
  
  // TODO (Dragon): this comparison uses the block size since the block and key size are the same
  //       for the undergrad version of the WSU-Crypt spec (64 bits)
  //
  //       THIS IS NOT ACCURATE OR SAFE WHEN CONVERTING KEYS IF KEY_SIZE != BLOCK_SIZE
  
  // make sure our size is fine
  if (size < 1 || size > BLOCK_SIZE) {
    return ERR_SIZE;
  }
  
  // make sure the buffers are good
  if (strbuff == NULL) {
    return ERR_BUFFER;
  }
  if (bytebuff == NULL) {
    return ERR_BUFFER;
  }
  
  // take the string 2 characters at a time to make a byte
  for (I32 i = 0; i < size*2; i+=2) {
    
    // TODO (Dragon): a little unintuitive calling character a byte here
    // make a string for the byte
    char a[2];// = {strbuff[i], strbuff[i+1]};
    strncpy(a, &(strbuff[i]), 2);
    
    // TODO (Dragon): make sure this is how strtol actually works
    // used to check errors after conversion
    STR e = "";
  
    // TODO (Dragon): get rid of this library call altogether and just do some math
    // make the final byte
    BYTE b = strtol(a, &e, 16);
    
    // TODO (Dragon): make sure this is how strtol actually works
    // make sure the characters were valid
    if (*e != '\0') {
      return ERR_CHAR;
    }
    
    // copy it to the byte buffer
    bytebuff[i/2] = b;
  }
  
  return ERR_OK;
}

// convert from a byte buffer of length size to a hex string buffer of length 2*size
ERR_TYPE bytes_hexstr(BYTE* bytebuff, STR strbuff, U32 size) {
  
  // TODO (Dragon): this comparison uses the block size since the block and key size are the same
  //       for the undergrad version of the WSU-Crypt spec (64 bits)
  //
  //       THIS IS NOT ACCURATE OR SAFE WHEN CONVERTING KEYS IF KEY_SIZE != BLOCK_SIZE
  
  // make sure our size is fine
  if (size < 1 || size > BLOCK_SIZE) {
    return ERR_SIZE;
  }
  
  // make sure the buffers are good
  if (strbuff == NULL) {
    return ERR_BUFFER;
  }
  if (bytebuff == NULL) {
    return ERR_BUFFER;
  }
  // string representation of all hex digits
  C_STR hexchars = "0123456789ABCDEF";
  
  // grab each byte and make it 2 characters
  for (I32 i = 0; i < size; i++) {
    // copy the current byte
    BYTE b = bytebuff[i];
    
    // convert and copy it to the string buffer
    strbuff[i*2] = hexchars[(b & 0xF0) >> 4];
    strbuff[(i*2)+1] = hexchars[b & 0x0F];
  }
  
  return ERR_OK;
}



//////////////////////////////////////////
// PRINTING

// TODO (Dragon): support logging to more than one file
// custom print helper
// mask fields:
//   bit 0: silent operation, ignores all other fields
//   bit 1: print critical errors (ERROR_MSG)
//   bit 2: print warnings (WARN_MSG)
//   bit 3: print info (INFO_MSG)
//   bit 4: print debug (DEBUG_MSG)
//   bit 5: print assert (ASSERT_MSG) (SHOULD ONLY BE CLEARED IN RELEASE BUILDS)
//   bit 6: print event (EVENT_MSG)
//   bit 7: print alert (ALERT_MSG)
void print_pretty(MSG_TYPE type, C_STR loc, C_STR msg, BF8 mask) {
  
  // return early for silent operation
  if (TEST_BIT_MASK(mask, P_SILENT)) {
    return;
  }
  
  // the final string eventually sent to fprintf()
  STR pretty_message = "[%s]: (%s): %s\n";
  char msg_tag[4];  // TODO (Dragon): unintuitive to call it a byte here
  
  // check the message type and apply appropriate tag
  switch(type) {

// INFO
    case INFO_MSG:
      // not printing info, return early
      if (!TEST_BIT_MASK(mask, P_INFO)) {
        return;
      }
      
      // set the current tag
      strncpy(msg_tag, INFO_MSG_TAG, TAG_SIZE);
      
      break;
    
// WARNING
    case WARN_MSG:
      // not printing warnings, return early
      if (!TEST_BIT_MASK(mask, P_WARN)) {
        return;
      }
      
      // set the current tag
      strncpy(msg_tag, WARN_MSG_TAG, TAG_SIZE);
      
      break;
    
// ERROR
    case ERROR_MSG:
      // not printing errors, return early
      if (!TEST_BIT_MASK(mask, P_ERROR)) {
        return;
      }
  
      // set the current tag
      strncpy(msg_tag, ERROR_MSG_TAG, TAG_SIZE);
      
      break;
    
// DEBUG
    case DEBUG_MSG:
      // not printing debug, return early
      if (!TEST_BIT_MASK(mask, P_DEBUG)) {
        return;
      }

      // set the current tag
      strncpy(msg_tag, DEBUG_MSG_TAG, TAG_SIZE);

      break;
    
// ASSERT
    case ASSERT_MSG:
      // not printing asserts, return early
      if (!TEST_BIT_MASK(mask, P_ASSERT)) {
        return;
      }
  
      // set the current tag
      strncpy(msg_tag, ASSERT_MSG_TAG, TAG_SIZE);
      
      break;
    
// EVENT
    case EVENT_MSG:
      // not printing events, return early
      if (!TEST_BIT_MASK(mask, P_EVENT)) {
        return;
      }
  
      // set the current tag
      strncpy(msg_tag, EVENT_MSG_TAG, TAG_SIZE);
      
      break;
      
// ALERT
    case ALERT_MSG:
      // not printing alerts, return early
      if (!TEST_BIT_MASK(mask, P_ALERT)) {
        return;
      }
  
      // set the current tag
      strncpy(msg_tag, ALERT_MSG_TAG, TAG_SIZE);
      
      break;
    
// UNKNOWN
    // intentionally falling through to default
    case UNKNOWN_MSG:
    default:
      // set the current tag
      strncpy(msg_tag, UNKNOWN_MSG_TAG, TAG_SIZE);
      
      break;
  }
  
  
  // TODO (Dragon): support printing to more than just stdout
  fprintf(stdout, pretty_message, msg_tag, loc, msg);

  return;
}


//////////////////////////////////////////
// RANDOM NUMBER GENERATION

// generate a random 32 bit number
U32 rand32() {
  // c standard rand doesnt spit out 32 bit values
  // so lets just make one
  U32 ret = rand() & 0xFF;
  //srand(ret ^ now.tv_usec);
  ret |= ((rand() & 0xFF) << 8);
  //srand(ret ^ now.tv_usec);
  ret |= ((rand() & 0xFF) << 16);
  //srand(ret ^ now.tv_usec);
  ret |= ((rand() & 0xFF) << 24);
  
  return ret;
}