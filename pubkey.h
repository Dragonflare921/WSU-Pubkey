/////////////////////////////////////////////////////////////////////
// Brandon Warner
// @dragonflare921
// dragonflare921@gmail.com
//
// WSU-Pubkey
//
//
// implementation of public key cryptosystem
//
//
// utilizes existing code provided by prime and exponent
//
//
// pubkey.h:
//  interface for the public key algorithm
//


// header guard
#ifndef _PUBKEY_H_
#define _PUBKEY_H_

#include "util.h"

// default paths
C_STR DEF_PLAIN_PATH = "ptext.txt";
C_STR DEF_CIPHER_PATH = "ctext.txt";
C_STR DEF_DECRYPT_PATH = "dtext.txt";
C_STR DEF_PUBKEY_PATH = "pubkey.txt";
C_STR DEF_PRIKEY_PATH = "prikey.txt";

// default verbosity
#ifdef DEBUG
BF8 DEF_VERBOSITY = 0x3E;
#else //NOT DEBUG
BF8 DEF_VERBOSITY = 0x0E;
#endif //DEBUG


// program mode
typedef enum PUBKEY_MODE {
  KEYGEN_MODE,
  ENCRYPT_MODE,
  DECRYPT_MODE,
  
  
  UNKNOWN_MODE
} PUBKEY_MODE;


// globals
typedef struct PUBKEY_GLOBALS {
  PUBKEY_MODE mode;       // program mode
  PATH plaintext_path;    // path to plaintext (default ptext.txt)
  PATH ciphertext_path;   // path to ciphertext (default ctext.txt)
  PATH decrypt_path;      // path to decrypted plaintext (default dtext.txt)
  PATH pubkey_path;       // path to public key (default pubkey.txt)
  PATH prikey_path;       // path to private key (default prikey.txt)
  BF8 verbosity;          // verbosity level
} PUBKEY_GLOBALS;


// generate a keypair and place them in respective files
ERR_TYPE generate_keys(C_STR pubpath, C_STR pripath);

// encrypt plaintext file and place in ciphertext file
ERR_TYPE encrypt(C_STR plainpath, C_STR cipherpath);

// decrypt ciphertext buffer and place in plaintext file
ERR_TYPE decrypt(C_STR cipherpath, C_STR plainpath);

#endif //_PUBKEY_H_
