/////////////////////////////////////////////////////////////////////
// Brandon Warner
// @dragonflare921
// dragonflare921@gmail.com
//
// WSU-Pubkey
//
//
// implementation of fast mod n exponentiation
//
// will use previously calculated exponents to calculate
// larger exponents faster. for example (from Stallings 7th, p36):
//
//  to find 11^7 mod n, we can proceed as follows
//  11^2 = 11 * 11
//  11^4 = (11^2)^2
//  11^7 = 11 * 11^2 * 11^4
//  
//
// exponent.c:
//  definition of the fast exponentiation algorithm
//

// shut the compiler up
#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "exponent.h"


// NOTE: it works, dont fucking touch it
// does a mod exponent using fast exponentiation of format: n^e mod m
U32 fast_mod_exp(U32 n, U32 e, U32 m, BF8 verbosity) {

#ifdef EXP_DEBUG
  STR msg = malloc(BUFF_SIZE);
  sprintf(msg, "called with\n  e = 0x%08X (%u)\n  n = 0x%08X (%u)\n  m = 0x%08X (%u)", e, e, n, n, m, m);
  print_pretty(DEBUG_MSG, __func__, msg, verbosity);
#endif //EXP_DEBUG

  // fun fUCKING FACT
  // things get bigger than 32 bits when you multiply
  // dont store it in a fucking 32 bit primitive
  U32 i = 0;
  U32 c = 0;
  I32 s = 31; // TODO (Dragon): things other than 32 bits
  U64 res = 1;
  FLAG b = (e >> (s)) & 0x01;
  
#ifdef EXP_DEBUG
  sprintf(msg, "iter %d\n  e = 0x%08X (%u)\n  n = 0x%08X (%u)\n  m = 0x%08X (%u)\n  c = 0x%08X (%u)\n  b = 0x%02X\n  r = 0x%08X (%lu)", i, e, e, n, n, m, m, c, c, b, res, res);
  print_pretty(DEBUG_MSG, __func__, msg, verbosity);
#endif //EXP_DEBUG  

  // come in from the top and mask every bit
  for (I32 s = 31; s >= 0; s--) {
    i++;
    b = (e >> (s)) & 0x01;
    c <<= 1;
    
#ifdef EXP_DEBUG
    sprintf(msg, "pre-square, iter %d\n  r = 0x%08X (%lu)", i, res, res);
    print_pretty(DEBUG_MSG, __func__, msg, verbosity);
#endif //EXP_DEBUG

    // do the square
    res = (U64)(res * res) % m;
    
#ifdef EXP_DEBUG
    sprintf(msg, "post-square, iter %d\n  r = 0x%08X (%lu)", i, res, res);
    print_pretty(DEBUG_MSG, __func__, msg, verbosity);
#endif //EXP_DEBUG
    
    // test the bit for odd exponent
    if (b) {
      c++;
      
#ifdef EXP_DEBUG
      sprintf(msg, "pre-multiply, iter %d\n  r = 0x%08X (%lu)", i, res, res);
      print_pretty(DEBUG_MSG, __func__, msg, verbosity);  
#endif //EXP_DEBUG

      // do the multiply
      res = (U64)(res * n) % m;
      
#ifdef EXP_DEBUG
      sprintf(msg, "post-multiply, iter %d\n  r = 0x%08X (%lu)", i, res, res);
      print_pretty(DEBUG_MSG, __func__, msg, verbosity);  
#endif //EXP_DEBUG
    }
    
#ifdef EXP_DEBUG
    sprintf(msg, "iter %d\n  e = 0x%08X (%u)\n  n = 0x%08X (%u)\n  m = 0x%08X (%u)\n  c = 0x%08X (%u)\n  b = 0x%02X\n  r = 0x%08X (%lu)", i, e, e, n, n, m, m, c, c, b, res, res);
    print_pretty(DEBUG_MSG, __func__, msg, verbosity);
#endif //EXP_DEBUG
  }
  
#ifdef EXP_DEBUG
  sprintf(msg, "result = 0x%08X (%lu)", res, res);
  print_pretty(DEBUG_MSG, __func__, msg, verbosity);
  free(msg);
#endif //EXP_DEBUG

  return res;
}


// skip compiling the entry and help if
// this is being built for use in prime
#ifdef EXP_STANDALONE

// prints help text
void printHelp() {
  printf("\
Fast modular exponentiation\n\
\n\
Brandon Warner\n\
@dragonflare921\n\
dragonflare921@gmail.com\n\
CS427, Spring 2019, Homework 4\n\
WSU Vancouver\n\
\n\
Performs a fast modular exponentiation by using\n\
previously calculated exponents in larger exponents\n\
to minimize computational complexity similar to\n\
dynamic programming or memoization.\n\
\n\
\n\
Usage:\n\
  ./exponent <NUMBER> <EXPONENT> <MODULO> [OPTIONS]\n\
\n\
Options:\n\
  -v          --verbose       Use verbose output\n\
  -h          --help          Show this help text\n\
\n");
  
  return;
}

// entry point
I32 main(I32 argc, STR* argv) {

  // default verbosity levels
#ifdef DEBUG
  BF8 verbosity = 0x3E;
#else //NOT DEBUG
  BF8 verbosity = 0x0E;
#endif

  // too few args
  if (argc < 4) {
    print_pretty(DEBUG_MSG, __func__, "Not enough args...", verbosity);
    printHelp();
    exit(EXIT_FAILURE);
  }
  
  
  // grab the arguments
  // TODO (Dragon): parse options
  
  // sanitize input a little with strtoul endptr checking
  STR eptr = "\0";
  
  // TODO (Dragon): use new printer
  // get number
  U32 number = (U32)strtoul(argv[1], &eptr, 10);
  if ((strcmp(argv[1], "\0") == 0) || !(strcmp(eptr, "\0") == 0)) {     // input sanity check
    fprintf(stderr, "[ERR!]: invalid digit(s) \'%s\' in arg 1 \'number\'. exiting...\n", eptr);
    exit(EXIT_FAILURE);
  } // get exponent
  U32 exponent = (U32)strtoul(argv[2], &eptr, 10);
  if ((strcmp(argv[2], "\0") == 0) || !(strcmp(eptr, "\0") == 0)) {     // input sanity check
    fprintf(stderr, "[ERR!]: invalid digit(s) \'%s\' in arg 2 \'exponent\'. exiting...\n", eptr);
    exit(EXIT_FAILURE);
  } // get mod
  U32 mod = (U32)strtoul(argv[3], &eptr, 10);
  if ((strcmp(argv[3], "\0") == 0) || !(strcmp(eptr, "\0") == 0)) {     // input sanity check
    fprintf(stderr, "[ERR!]: invalid digits(s) \'%s\' in arg 3 \'mod\'. exiting...\n", eptr);
    exit(EXIT_FAILURE);
  }
  
  
  // calculate and print result
  U32 result = fast_mod_exp(number, exponent, mod, verbosity);
  printf("result of %d^%d mod %d:\n  %d\n", number, exponent, mod, result);
  return 0;
}

#endif //EXP_STANDALONE
