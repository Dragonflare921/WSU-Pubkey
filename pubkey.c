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
// pubkey.c:
//  definition of the public key algorithm
//


// shut the compiler up
#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "pubkey.h"
#include "exponent.h"
#include "prime.h"


// globals are packed in a struct
PUBKEY_GLOBALS* _GLOBALS;


// generate a keypair and place them in respective files
ERR_TYPE generate_keys(C_STR pubpath, C_STR pripath) {
  
  // TODO (Dragon): this is stupid, do something better
  // buffer for sprintf formatted messages
  // which get sent to pretty_printer
  STR msg = calloc(1, BUFF_SIZE);
  

  // handle seeing the RNG
  STR seed_str = NULL;
  U32 seed_str_len = 0;
  U32 seed;
  
// support hashing a random string, or just using a number directly
// can change at compile time, but expects number by default
#ifdef STRING_SEED
  // get the user to mash some keys
  printf("Enter a random string. Slap your keyboard a bit\n");
  getline(&seed_str, &seed_str_len, stdin);
  seed_str_len = strlen(seed_str);
  
  // get rid of the newline
  seed_str[seed_str_len-1] = "\0";
  
  // mix it up a little
  //
  // tbh i made stir fry for dinner and just want an 
  // excuse to shoehorn a call to strfry() in here lol
  STR mash_fry = strfry(seed_str);

#ifdef PUB_KEYS_DEBUG
  sprintf(msg, "Using seed string sized %d: \'%s\'", seed_str_len, mash_fry);
  print_pretty(DEBUG_MSG, __func__, msg, _GLOBALS->verbosity);
#endif //PUB_KEYS_DEBUG
  
  // do some hashing on the string
  // using hash function by Dan Bernstein
  // http://www.cse.yorku.ca/~oz/hash.html
  seed = djb2_hash(mash_fry);
  
  
#else //NOT STRING_SEED
  // prompt for input
  printf("Enter a seed value:\n");
  
  // grab the line
  getline(&seed_str, (size_t*)&seed_str_len, stdin);
  seed_str_len = strlen(seed_str);
  
  // get rid of the newline
  seed_str[seed_str_len-1] = 0;
  
  // sanitize the input
  STR eptr = "\0";
  
  // parse the seed from the input string
  seed = (U32)strtol(seed_str, &eptr, 10);
  
  // TODO (Dragon): can probably make this better so we can get rid of reused lines
  while ((strcmp(seed_str, "\0") == 0) || !(strcmp(eptr, "\0") == 0)) {
    sprintf(msg, "Invalid digit(s) \'%s\'. Please use valid seed.", eptr);
    print_pretty(ERROR_MSG, __func__, msg, _GLOBALS->verbosity);
    
    // reset
    //seed_str = NULL;
    
    // grab the line
    getline(&seed_str, (size_t*)&seed_str_len, stdin);
    seed_str_len = strlen(seed_str);
  
    // get rid of the newline
    seed_str[seed_str_len-1] = 0;
    
    // sanitize the input
    eptr = "\0";
    
    // parse the seed from the input string
    seed = (U32)strtol(seed_str, &eptr, 10);
  }
#endif

  sprintf(msg, "Using seed value: %d", seed);
  print_pretty(INFO_MSG, __func__, msg, _GLOBALS->verbosity);

  // NOTE: NEED THIS *HERE*
  // use the input to seed RNG
  srand(seed);
  
  // generate P
  // using the algorithm from "Handbook of Applied Cryptography"
  // select random k-1 bit prime q and test with miller rabin
  // so that q mod 12 = 5
  //
  // then computer  p = 2q + 1 and test whether p is prime
  // again with miller rabin
  //
  // repeat until p is prime
  //
  // using 32 bit prime, and 31 bit message block
  // using wrapper for C standard random
  // (i know theres better PRNG)
  U64 q;
  U64 p;
  FLAG done1 = false;
  FLAG done2 = false;
  
  // keep trying until p is a valid prime
  while (!done1) {
  
    // keep trying new values for q until we get one that works
    while (!done2) {
qloop:
      q = rand32();
      if (isprime(q, 10, false) && ((q % 12) == 5)) {
#ifdef PUB_KEYS_DEBUG
        sprintf(msg, "Good q: 0x%08lX", q);
        print_pretty(DEBUG_MSG, __func__, msg, _GLOBALS->verbosity);
#endif //PUB_KEYS_DEBUG
        done2 = true;
      }
      else {
#ifdef PUB_KEYS_DEBUG
        sprintf(msg, "Bad q: 0x%08lX", q);
        print_pretty(DEBUG_MSG, __func__, msg, _GLOBALS->verbosity);
#endif //PUB_KEYS_DEBUG
      }
    }
    
    // calculate p
    p = (2 * q) + 1;
    
    // only exit if p is prime with high bit set
    if ((p & (1<<31)) && isprime(p, 10, false)) {
#ifdef PUB_KEYS_DEBUG
      sprintf(msg, "Good p: 0x%08lX", p);
      print_pretty(DEBUG_MSG, __func__, msg, _GLOBALS->verbosity);
#endif //PUB_KEYS_DEBUG
      done1 = true;
    }
    else {
#ifdef PUB_KEYS_DEBUG
      sprintf(msg, "Bad p: 0x%08lX", p);
      print_pretty(DEBUG_MSG, __func__, msg, _GLOBALS->verbosity);
#endif //PUB_KEYS_DEBUG
      done2 = false;
      goto qloop;   // listen. this is the first goto ive done in like a decade
    }
  }
  
  // using generator g = 2
  U64 g = 2;
  
  // pick a random private key d where 0 < d < p
  U64 d = 1 + (rand32() % (p - 2));
  
  
  // calculate public key e (E2 in spec)
  U64 e = fast_mod_exp(g, d, p, _GLOBALS->verbosity);
  
  // print keys for ease of use/grading
  sprintf(msg, "Generated:\n  p: 0x%08lX (%lu)\n  g: 0x%08lX (%lu)\n  e: 0x%08lX (%lu)\n  d: 0x%08lX (%lu)", p, p, g, g, e, e, d, d);
  print_pretty(INFO_MSG, __func__, msg, _GLOBALS->verbosity);
  
  // open keyfiles
  FILE* pubfile;
  FILE* prifile;
  
  // handle error
  if ((pubfile = fopen(pubpath, "wb")) == NULL) {
    print_pretty(ERROR_MSG, __func__, "Couldnt open pubkey file", _GLOBALS->verbosity);
    return ERR_OPEN;
  }
  // handle error
  if ((prifile = fopen(pripath, "wb")) == NULL) {
    print_pretty(ERROR_MSG, __func__, "Couldnt open prikey file", _GLOBALS->verbosity);
    return ERR_OPEN;
  }
  
  // both files should be successfully opened at this point
  
  
  
  // get key value string lengths using an snprintf trick
  // from stackoverflow: https://stackoverflow.com/a/32819876
  U32 pub_len = snprintf(NULL, 0, "%lu %lu %lu", p, g, e);
  U32 pri_len = snprintf(NULL, 0, "%lu %lu %lu", p, g, d);
  
  STR pub_out = malloc(pub_len+1);
  STR pri_out = malloc(pri_len+1);
  
  // format the output strings
  snprintf(pub_out, pub_len+1, "%lu %lu %lu", p, g, e);
  snprintf(pri_out, pri_len+1, "%lu %lu %lu", p, g, d);
  
  // write output
  fwrite(pub_out, pub_len+1, 1, pubfile);
  fwrite(pri_out, pri_len+1, 1, prifile);
  
  // clean up
  free(msg);
  free(seed_str);
  free(pub_out);
  free(pri_out);
  
  fclose(pubfile);
  fclose(prifile);
  
  return ERR_OK;
}

// encrypt plaintext buffer and place in ciphertext buffer
ERR_TYPE encrypt(C_STR plainpath, C_STR cipherpath) {
  STR msg = malloc(BUFF_SIZE);
  
  FILE* plainfile = fopen(plainpath, "r");
  if (plainfile == NULL) {
    sprintf(msg, "Couldnt open plaintext file \'%s\'", plainpath);
    print_pretty(ERROR_MSG, __func__, msg, _GLOBALS->verbosity);
    return ERR_OPEN;
  }
  
  FILE* cipherfile = fopen(cipherpath, "w");
  if (cipherfile == NULL) {
    sprintf(msg, "Couldnt open ciphertext file \'%s\'", cipherpath);
    print_pretty(ERROR_MSG, __func__, msg, _GLOBALS->verbosity);
    return ERR_OPEN;
  }
  
  // grab the length of the plaintext for later
  fseek(plainfile, 0, SEEK_END);
  U32 plainlen = ftell(plainfile);
  
#ifdef ENC_DEBUG
  sprintf(msg, "plaintext input length 0x%08X (%u)", plainlen, plainlen);
  print_pretty(DEBUG_MSG, __func__, msg, _GLOBALS->verbosity);
#endif //ENC_DEBUG
  
  rewind(plainfile);
  
  // open keyfiles
  FILE* pubfile;
  FILE* prifile;
  
  // handle error
  if ((pubfile = fopen(_GLOBALS->pubkey_path, "r")) == NULL) {
    print_pretty(ERROR_MSG, __func__, "Couldnt open pubkey file", _GLOBALS->verbosity);
    return ERR_OPEN;
  }
  // handle error
  if ((prifile = fopen(_GLOBALS->prikey_path, "r")) == NULL) {
    print_pretty(ERROR_MSG, __func__, "Couldnt open prikey file", _GLOBALS->verbosity);
    return ERR_OPEN;
  }
  
  // load keys from file strings
  STR pub_line = "";
  //STR pri_line;
  size_t len = 0;
  
  // keys are stored on first line, delimited by space
  getline(&pub_line, &len, pubfile);
  //getline(&pri_line, &len, prifile);
  
#ifdef ENC_DEBUG
  sprintf(msg, "pubkey file \'%s\' contains line:\n  \'%s\'", _GLOBALS->pubkey_path, pub_line);
  print_pretty(DEBUG_MSG, __func__, msg, _GLOBALS->verbosity);
#endif //END_DEBUG
  
  STR p_str = strtok(pub_line, " ");
  STR g_str = strtok(NULL, " ");
  STR e_str = strtok(NULL, " ");
  
  
  
  // sanitize input a little with strtoul endptr checking
  STR eptr = "\0";
  
  U32 p = strtoul(p_str, &eptr, 10);
  if ((strcmp(p_str, "\0") == 0) || !(strcmp(eptr, "\0") == 0)) {     // input sanity check
    sprintf(msg, "invalid digit(s) \'%s\' in \'p\'. exiting...\n", eptr);
    print_pretty(ERROR_MSG, __func__, msg, _GLOBALS->verbosity);
    exit(EXIT_FAILURE);
  }
  U32 g = strtoul(g_str, &eptr, 10);
  if ((strcmp(g_str, "\0") == 0) || !(strcmp(eptr, "\0") == 0)) {     // input sanity check
    sprintf(msg, "invalid digit(s) \'%s\' in \'g\'. exiting...\n", eptr);
    print_pretty(ERROR_MSG, __func__, msg, _GLOBALS->verbosity);
    exit(EXIT_FAILURE);
  }
  U32 e = strtoul(e_str, &eptr, 10);
  if ((strcmp(e_str, "\0") == 0) || !(strcmp(eptr, "\0") == 0)) {     // input sanity check
    sprintf(msg, "invalid digit(s) \'%s\' in \'e\'. exiting...\n", eptr);
    print_pretty(ERROR_MSG, __func__, msg, _GLOBALS->verbosity);
    exit(EXIT_FAILURE);
  }
  
#ifdef ENC_DEBUG
  sprintf(msg, "pubkey vals:\n  p = 0x%08X (%u)\n  g = 0x%08X (%u)\n  e = 0x%08X (%u))", p, p, g, g, e, e);
  print_pretty(DEBUG_MSG, __func__, msg, _GLOBALS->verbosity);
#endif //ENC_DEBUG


  // prime size is 32 bits, block size is 31 bits
  BYTE m_block[4];
  BYTE c_block[8];
  for (I32 i = 0; i < plainlen; i += 4) {
    
    // read next 32 bits from plaintext
    fread(m_block, 4, 1, plainfile);
    
#ifdef ENC_DEBUG
    sprintf(msg, "m_block: %s", m_block);
    print_pretty(DEBUG_MSG, __func__, msg, _GLOBALS->verbosity);
#endif //ENC_DEBUG
  
    // random K for every block
    U64 k = rand32() % p;

    // encrypt plaintext block
    U64 c1 = fast_mod_exp(g, k, p, _GLOBALS->verbosity);
    U64 c2 = fast_mod_exp(e, k, p, _GLOBALS->verbosity);
    c2 = (U64)(c2 * *((U32*)m_block)) % p;
    //U32 c3 = (U32)c1;
    //U32 c4 = (U32)c2;
    
    // move ciphertext values into the same buffer to write
    memcpy(&c_block[0], &c1, 4);
    memcpy(&c_block[4], &c2, 4);
  
#ifdef ENC_DEBUG
    sprintf(msg, "blocks:\n  m_block = 0x%08X (%u)\n  c_block = 0x%016lX  (%lu)\n  k = 0x%08lX (%lu)\n  c1 = 0x%08lX (%lu)\n  c2 = 0x%08lX (%lu)", *((U32*)m_block), *((U32*)m_block), *((U64*)c_block), *((U64*)c_block), k, k, c1, c1, c2, c2);
    print_pretty(DEBUG_MSG, __func__, msg, _GLOBALS->verbosity);
#endif //ENC_DEBUG
  
    sprintf(msg, "block %d C values:\n  k = 0x%08lX (%lu)\n  c1 = 0x%08lX (%lu)\n  c2 = 0x%08lX (%lu)", i/4, k, k, c1, c1, c2, c2);
    print_pretty(INFO_MSG, __func__, msg, _GLOBALS->verbosity);
  
    // write result to ciphertext
    fwrite(c_block, 8, 1, cipherfile);
    
    // clean the buffers out
    memset(m_block, 0, 4);
    memset(c_block, 0, 8);
  }
  
  
  fclose(pubfile);
  fclose(prifile);
  fclose(plainfile);
  fclose(cipherfile);
  free(msg);
  free(pub_line);

  
  return ERR_OK;
}

// decrypt ciphertext buffer and place in plaintext buffer
ERR_TYPE decrypt(C_STR cipherpath, C_STR plainpath) {
  STR msg = malloc(BUFF_SIZE);
  
  FILE* plainfile = fopen(plainpath, "w+");
  if (plainfile == NULL) {
    sprintf(msg, "Couldnt open plaintext file \'%s\'", plainpath);
    print_pretty(ERROR_MSG, __func__, msg, _GLOBALS->verbosity);
    return ERR_OPEN;
  }
  
  FILE* cipherfile = fopen(cipherpath, "rb");
  if (cipherfile == NULL) {
    sprintf(msg, "Couldnt open ciphertext file \'%s\'", cipherpath);
    print_pretty(ERROR_MSG, __func__, msg, _GLOBALS->verbosity);
    return ERR_OPEN;
  }
  
  // grab the length of the plaintext for later
  fseek(cipherfile, 0, SEEK_END);
  U32 cipherlen = ftell(cipherfile);
  
#ifdef DEC_DEBUG
  sprintf(msg, "ciphertext input length 0x%08X (%u)", cipherlen, cipherlen);
  print_pretty(DEBUG_MSG, __func__, msg, _GLOBALS->verbosity);
#endif //DEC_DEBUG
  
  rewind(cipherfile);
  
  // open keyfiles
  FILE* pubfile;
  FILE* prifile;
  
  // handle error
  if ((pubfile = fopen(_GLOBALS->pubkey_path, "r")) == NULL) {
    print_pretty(ERROR_MSG, __func__, "Couldnt open pubkey file", _GLOBALS->verbosity);
    return ERR_OPEN;
  }
  // handle error
  if ((prifile = fopen(_GLOBALS->prikey_path, "r")) == NULL) {
    print_pretty(ERROR_MSG, __func__, "Couldnt open prikey file", _GLOBALS->verbosity);
    return ERR_OPEN;
  }
  
  // load keys from file strings
  //STR pub_line = "";
  STR pri_line = "";
  size_t len = 0;
  
  // keys are stored on first line, delimited by space
  //getline(&pub_line, &len, pubfile);
  getline(&pri_line, &len, prifile);
  
  sprintf(msg, "prikey file \'%s\' contains line:\n  \'%s\'", _GLOBALS->prikey_path, pri_line);
  print_pretty(DEBUG_MSG, __func__, msg, _GLOBALS->verbosity);
  
  STR p_str = strtok(pri_line, " ");
  STR g_str = strtok(NULL, " ");
  STR d_str = strtok(NULL, " ");
  
  
  
  // sanitize input a little with strtoul endptr checking
  STR eptr = "\0";
  
  U64 p = strtoul(p_str, &eptr, 10);
  if ((strcmp(p_str, "\0") == 0) || !(strcmp(eptr, "\0") == 0)) {     // input sanity check
    sprintf(msg, "invalid digit(s) \'%s\' in \'p\'. exiting...\n", eptr);
    print_pretty(ERROR_MSG, __func__, msg, _GLOBALS->verbosity);
    exit(EXIT_FAILURE);
  }
  U64 g = strtoul(g_str, &eptr, 10);
  if ((strcmp(g_str, "\0") == 0) || !(strcmp(eptr, "\0") == 0)) {     // input sanity check
    sprintf(msg, "invalid digit(s) \'%s\' in \'g\'. exiting...\n", eptr);
    print_pretty(ERROR_MSG, __func__, msg, _GLOBALS->verbosity);
    exit(EXIT_FAILURE);
  }
  U64 d = strtoul(d_str, &eptr, 10);
  if ((strcmp(d_str, "\0") == 0) || !(strcmp(eptr, "\0") == 0)) {     // input sanity check
    sprintf(msg, "invalid digit(s) \'%s\' in \'d\'. exiting...\n", eptr);
    print_pretty(ERROR_MSG, __func__, msg, _GLOBALS->verbosity);
    exit(EXIT_FAILURE);
  }
  
#ifdef DEC_DEBUG
  sprintf(msg, "pubkey vals:\n  p = 0x%08lX (%lu)\n  g = 0x%08lX (%lu)\n  d = 0x%08lX (%lu))", p, p, g, g, d, d);
  print_pretty(DEBUG_MSG, __func__, msg, _GLOBALS->verbosity);
#endif //DEC_DEBUG


  // prime size is 32 bits, block size is 31 bits
  BYTE m_block[4];
  BYTE c_block[8];
  for (I32 i = 0; i < cipherlen; i += 8) {
    
    // read next 64 bits from plaintext
    fread(c_block, 8, 1, cipherfile);
    
#ifdef DEC_DEBUG
    sprintf(msg, "c_block: %s", c_block);
    print_pretty(DEBUG_MSG, __func__, msg, _GLOBALS->verbosity);
#endif //DEC_DEBUG

    
    // decrypt ciphertext block
    U64 c1 = 0;
    U64 c2 = 0;
    memcpy(&c1, &c_block[0], 4);
    memcpy(&c2, &c_block[4], 4);
    
#ifdef DEC_DEBUG
    sprintf(msg, "ciphertext blocks:\n  c1 = 0x%08lX (%lu)\n  c2 = 0x%08lX (%lu)", c1, c1, c2, c2);
    print_pretty(DEBUG_MSG, __func__, msg, _GLOBALS->verbosity);
#endif //DEC_DEBUG
    
    U64 m1 = 0;
    m1 = fast_mod_exp(c1, p - 1 - d, p, _GLOBALS->verbosity);
    U64 m2 = 0;
    m2 = c2 % p;
    
    U64 m = 0;
    m = (U64)(m1*m2) % p;
    //U32 m32 = (U32)m;
    
#ifdef DEC_DEBUG
    sprintf(msg, "message values:\n  m1 = 0x%08lX (%lu)\n  m2 = 0x%08lX (%lu)\n  m = 0x%08lX (%lu)", m1, m1, m2, m2, m, m);
    print_pretty(DEBUG_MSG, __func__, msg, _GLOBALS->verbosity);
#endif //DEC_DEBUG
    
    memcpy(&m_block[0], &m, 4);

#ifdef DEC_DEBUG
    sprintf(msg, "m_block = %s", m_block);
    print_pretty(DEBUG_MSG, __func__, msg, _GLOBALS->verbosity);
#endif //DEC_DEBUG
  
    // write result to plaintext
    fwrite(m_block, 1, 4, plainfile);
    
    
    
    
    
    // clean the buffers out
    memset(m_block, 0, 4);
    memset(c_block, 0, 8);
    
    
    
    
    
  }
  
  // print the dtext to the screen
  // locals for line processing
  STR line = "";
  U32 charsread = 0;
  size_t dlen = 0;
  rewind(plainfile);

  sprintf(msg, "decrypted text \'%s\' follows", plainpath);
  print_pretty(INFO_MSG, __func__, msg, _GLOBALS->verbosity);
  
  // loop over each line
  while ((charsread = getline(&line, &dlen, plainfile)) != -1) {
    printf("%s", line);
  }

  
  fclose(pubfile);
  fclose(prifile);
  fclose(plainfile);
  fclose(cipherfile);
  free(msg);
  free(line);
  free(pri_line);
  
  return ERR_OK;
}


// TODO (Dragon): wrap in an ifdef for future portability

// TODO (Dragon): handle errors maybe?
void init(PUBKEY_GLOBALS* globals) {
  // set up globals
  globals->mode = UNKNOWN_MODE;   // no default mode. MUST be supplied at runtime
  strncpy(globals->plaintext_path, DEF_PLAIN_PATH, MAX_PATH);   // default path to plaintext
  strncpy(globals->ciphertext_path, DEF_CIPHER_PATH, MAX_PATH); // default path to ciphertext
  strncpy(globals->decrypt_path, DEF_DECRYPT_PATH, MAX_PATH);   // default path to decrypted plaintext
  strncpy(globals->pubkey_path, DEF_PUBKEY_PATH, MAX_PATH);     // default path to public key
  strncpy(globals->prikey_path, DEF_PRIKEY_PATH, MAX_PATH);     // default path to private key
  globals->verbosity = DEF_VERBOSITY; // default verbosity
}

// prints help text
void printHelp() {
  printf("\
Public Key Cryptosystem\n\
\n\
Brandon Warner\n\
@dragonflare921\n\
dragonflare921@gmail.com\n\
CS427, Spring 2019, Project 2\n\
WSU Vancouver\n\
\n\
Public key cryptosystem which supports key generation,\n\
encryption, and decryption using command line options.\n\
\n\
By default, expects plaintext to be in \'ptext.txt\',\n\
public key in \'pubkey.txt\', and private key in \'prikey.txt\'\n\
\n\
\n\
Usage:\n\
  ./pubkey <MODE> [OPTIONS]\n\
\n\
Modes:\n\
  keygen        Generate keys with some random user input\n\
  encrypt       Encrypt the plaintext file using keys\n\
  decrypt       Decrypt the plaintext file using keys\n\
\n\
Options:\n\
  -v        --verbosity       Use verbose output\n\
  -h        --help            Show this help text\n\
\n\
            --plain   <PATH>  Use a different input plaintext\n\
            --cipher  <PATH>  Output a different ciphertext\n\
            --public  <PATH>  Use a different public key\n\
            --private <PATH>  Use a different private key\n\
\n");
  
  return;
}


// TODO (Dragon): move this into util somehow
//       cause uh, hot damn. this is getting robust
// TODO (Dragon): do we want to pass the globals in?
//       it does make it safer, but everything else
//       just hits it directly from the reference up top
// parse arguments
ERR_TYPE parse_args(I32 argc, STR* argv, PUBKEY_GLOBALS* globals) {
  
  // loop through options first
  for (I32 i = 1; i < argc; i++) {
    print_pretty(DEBUG_MSG, __func__, argv[i], globals->verbosity);
    
    // check help flag
    if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
      printHelp();
      exit(EXIT_SUCCESS);
    }
    
    // check verbosity level
    if ((strcmp(argv[i], "-v") == 0) || (strcmp(argv[i], "--verbosity") == 0)) {
      // copy the level
      char level[4];
      strncpy(level, argv[i++], 4);
      
      // make sure it starts with 0x
      if (strncmp("0x", level, 2) == 0) {
        // convert hexstr to a byte
        BYTE level_byte;
        hexstr_bytes(level, &level_byte, 1);
        globals->verbosity = level_byte;
      }
      else {
        print_pretty(ERROR_MSG, __func__, "Invalid verbosity", globals->verbosity);
        return ERR_VERBOSITY;
      }
    }
    
    // check public key file
    if ((strcmp(argv[i], "--public") == 0)) {
      strncpy(globals->pubkey_path, argv[i++], MAX_PATH);
    }
    
    // check private key file
    if ((strcmp(argv[i], "--private") == 0)) {
      strncpy(globals->prikey_path, argv[i++], MAX_PATH);
    }
    
    // check plaintext file
    if ((strcmp(argv[i], "--plain") == 0)) {
      strncpy(globals->plaintext_path, argv[i++], MAX_PATH);
    }  
    
    // check ciphertext file
    if ((strcmp(argv[i], "--cipher") == 0)) {
      strncpy(globals->ciphertext_path, argv[i++], MAX_PATH);
    }
  }
  
  // get the mode after checking options
  
  // MODE MUST BE FIRST ARG
  // MUST PROVIDE MODE
  // keygen mode
  if (strcmp(argv[1], "keygen") == 0) {
#ifdef DEBUG
    print_pretty(DEBUG_MSG, __func__, "Keygen mode", globals->verbosity);
#endif //DEBUG
    globals->mode = KEYGEN_MODE;
  }
  // encryption mode
  else if (strcmp(argv[1], "encrypt") == 0) {
#ifdef DEBUG
    print_pretty(DEBUG_MSG, __func__, "Encryption mode", globals->verbosity);
#endif //DEBUG
    globals->mode = ENCRYPT_MODE;
  }
  // decryption mode
  else if (strcmp(argv[1], "decrypt") == 0) {
#ifdef DEBUG
    print_pretty(DEBUG_MSG, __func__, "Decryption mode", globals->verbosity);
#endif //DEBUG
    globals->mode = DECRYPT_MODE;
  }
  // bad mode/no mode
  else {
    print_pretty(ERROR_MSG, __func__, "Invalid mode", globals->verbosity);
    return ERR_MODE;
  }
  
  return ERR_OK;
}


// entry point
int main(int argc, char** argv) {

  _GLOBALS = malloc(sizeof(PUBKEY_GLOBALS));
  STR msg = malloc(BUFF_SIZE);
  
  // NOTE: i know this is a bad seed
  //       its just to catch anything
  //       that doesnt get seeded elsewhere
  srand(time(NULL));
  
  // initialize globals
  init(_GLOBALS);

  // too few args
  if (argc < 2) {
    print_pretty(ERROR_MSG, __func__, "Not enough args", _GLOBALS->verbosity);
    printHelp();
    exit(EXIT_FAILURE);
  }
  
  // parse arguments
  parse_args(argc, argv, _GLOBALS);
  
  // keygen mode
  if (_GLOBALS->mode == KEYGEN_MODE) {
    generate_keys(_GLOBALS->pubkey_path, _GLOBALS->prikey_path);
  }
  
  // encrypt mode
  else if (_GLOBALS->mode == ENCRYPT_MODE) {
    encrypt(_GLOBALS->plaintext_path, _GLOBALS->ciphertext_path);
  }
  
  // decrypt mode
  else if (_GLOBALS->mode == DECRYPT_MODE) {
    decrypt(_GLOBALS->ciphertext_path, _GLOBALS->decrypt_path);
  }
  
  
  // clean up
  free(msg);
  free(_GLOBALS);
  
  return 0;
}