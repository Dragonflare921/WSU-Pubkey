/////////////////////////////////////////////////////////////////////
// Brandon Warner
// @dragonflare921
// dragonflare921@gmail.com
//
// WSU-Pubkey
//
//
// implementation of the miller-rabin algorithm
//
// will determine with 100% certainty whether a number
// is *not* prime, but cannot determine whether a number
// *is* prime. using repeated applications, the margin of
// uncertainty becomes negligible
//
//
// prime.c:
//  definition of the miller rabin algorithm
//

// shut the compiler up
#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "exponent.h"

#define true  0x01
#define false 0x00



// miller-rabin primality helper
FLAG witness(U64 u, U64 n, BF8 verbosity) {

#ifdef WIT_DEBUG
  STR msg = malloc(BUFF_SIZE);
  sprintf(msg, "called with\n  u = 0x%08X\n  n = 0x%08X", u, n);
  print_pretty(DEBUG_MSG, __func__, msg, verbosity);
#endif //WIT_DEBUG
  
  U64 a = 2 + (rand32() % (n - 4));  
  U64 x = fast_mod_exp(a, u, n, verbosity);

#ifdef WIT_DEBUG
  sprintf(msg, "initial:\n  a = 0x%08X\n  x = 0x%08X\n  n = 0x%08X\n  u = 0x%08X", a, x, n, u);
  print_pretty(DEBUG_MSG, __func__, msg, verbosity);
#endif //WIT_DEBUG
  
  U32 i = 0;
  
  if (x == 1 || x == (n - 1)) {
#ifdef WIT_DEBUG
    print_pretty(DEBUG_MSG, __func__, "x == n-1 || x == 1, return *TRUE*", verbosity);
#endif //WIT_DEBUG
    return true;
  }
  
  while (u != (n - 1)) {
    //x = fast_mod_exp(x, 2, n, verbosity);
    x = (U64)(x * x) % (U64)n;
    u = u * 2;

#ifdef WIT_DEBUG
    sprintf(msg, "iter %d\n  a = 0x%08X\n  x = 0x%08X\n  n = 0x%08X\n  u = 0x%08X", i, a, x, n, u);
    print_pretty(DEBUG_MSG, __func__, msg, verbosity);
#endif //WIT_DEBUG
    
    if (x == 1) {
#ifdef WIT_DEBUG
      print_pretty(DEBUG_MSG, __func__, "x == 1, return *FALSE*", verbosity);
#endif //WIT_DEBUG
      return false;
    }
    if (x == (n - 1)) {
#ifdef WIT_DEBUG
      print_pretty(DEBUG_MSG, __func__, "x == n-1, return *TRUE*", verbosity);
#endif //WIT_DEBUG
      return true;
    }
    
    i++;
  }
  
#ifdef WIT_DEBUG
  free(msg);
  print_pretty(DEBUG_MSG, __func__, "final return *FALSE*", verbosity);
#endif //WIT_DEBUG

  return false;
}

// takes a number n and iterations i and returns false if
// n is not prime, but returning true does not guarantee primality
FLAG isprime(U64 n, U32 i, BF8 verbosity) {

#ifdef PRIME_DEBUG
  STR msg = malloc(BUFF_SIZE);
#endif //PRIME_DEBUG
  
// quickly discard 1, and factors of 2, 3, and 5
#ifdef QUICK_DISCARD
  // 1 is a prime
  if (n == 1) {
#ifdef PRIME_DEBUG
    print_pretty(DEBUG_MSG, __func__, "early *TRUE* return for 1", verbosity);
#endif //PRIME_DEBUG
    return true;
  }
  
  // 2 is a prime
  if (n == 2) {
#ifdef PRIME_DEBUG
    print_pretty(DEBUG_MSG, __func__, "early *TRUE* return for 2", verbosity);
#endif //PRIME_DEBUG
    return true;
  } // factors of 2 are not primes
  else if (n % 2 == 0) {
#ifdef PRIME_DEBUG
    print_pretty(DEBUG_MSG, __func__, "early *FALSE* return for factor of 2", verbosity);
#endif //PRIME_DEBUG
    return false;
  }
  
  // 3 is a prime
  if (n == 3) {
#ifdef PRIME_DEBUG
    print_pretty(DEBUG_MSG, __func__, "early *TRUE* return for 3", verbosity);
#endif //PRIME_DEBUG
    return true;
  } // factors of 3 are not primes
  else if (n % 3 == 0) {
#ifdef PRIME_DEBUG
    print_pretty(DEBUG_MSG, __func__, "early *FALSE* return for factor of 3", verbosity);
#endif //PRIME_DEBUG
    return false;
  }
  
  // 5 is a prime
  if (n == 5) {
#ifdef PRIME_DEBUG
    print_pretty(DEBUG_MSG, __func__, "early *TRUE* return for 5", verbosity);
#endif //PRIME_DEBUG
    return true;
  } // factors of 5 are not primes
  else if (n % 5 == 0) {
#ifdef PRIME_DEBUG
    print_pretty(DEBUG_MSG, __func__, "early *FALSE* return for factor of 5", verbosity);
#endif //PRIME_DEBUG
    return false;
  }

#endif //QUICK_DISCARD


  // find t and u such that t >= 1, u is odd, and n-1 = (2**t)u
  U64 u = n - 1;

  // calculate the t and u values
  while (u % 2 == 0) {
    u /= 2;
    
#ifdef PRIME_DEBUG
    sprintf(msg, "calculating u = 0x%08X", u);
    print_pretty(DEBUG_MSG, __func__, msg, verbosity);
#endif //PRIME_DEBUG
  }

#ifdef PRIME_DEBUG
  sprintf(msg, "checking 0x%08X with %d iters, u = 0x%08X", n, i, u);
  print_pretty(DEBUG_MSG, __func__, msg, verbosity);
#endif//PRIME_DEBUG
  
  // iterations of miller-rabin
  for (U32 j = 0; j < i; j++) {
#ifdef PRIME_DEBUG
    sprintf(msg, "iter: %d of %d\n", j, i);
    print_pretty(DEBUG_MSG, __func__, msg, verbosity);
#endif //PRIME_DEBUG

    // call the helper
    if (!witness(u, n, verbosity)) {
      return false;   // definitely not prime
    }
  }

#ifdef PRIME_DEBUG
  free(msg);
#endif //PRIME_DEBUG

  return true;  // might be prime
}


// skip compiling the entry and help if
// this is being built for use in pubkey
#ifdef PRIME_STANDALONE

// prints help text
void printhelp() {
  printf("\
Miller-Rabin primality\n\
\n\
Brandon Warner\n\
@dragonflare921\n\
dragonflare921@gmail.com\n\
CS427, Spring 2019, Homework 4\n\
WSU Vancouver\n\
\n\
Implementation of the Miller-Rabin primality test\n\
Guaranteed to find if a number *isnt* prime,\n\
but not guaranteed to find if it *is*.\n\
This uncertainty becomes negligible after\n\
enough repetitions.\n\
\n\
\n\
Usage:\n\
  ./prime <NUMBER> <ITERATIONS> [OPTIONS]\n\
\n\
Options:\n\
  -f <FNAME>  --file <FNAME>  Use an input list of primes\n\
  -v          --verbose       Use verbose output\n\
  -h          --help          Show this help text\n\
\n");
  
  return;
}

// entry point
int main(int argc, char** argv) {

  // seed the rng
  time_t now = time(0);
  srand(now);

  // too few args
  if (argc < 3) {

#ifdef DEBUG
  printf("[DBUG]: not enough args\n");
#endif //DEBUG
    
    printhelp();
    exit(EXIT_FAILURE);
  }
  
  // options
  char* filepath = "";
  // default verbosity levels
#ifdef DEBUG
  BF8 verbosity = 0x3E;
#else //NOT DEBUG
  BF8 verbosity = 0x0E;
#endif
  
  // grab the arguments
  for (int i = 0; i < argc; i++) {
    // using files
    if ((strcmp(argv[i], "-f") == 0) || (strcmp(argv[i], "--file") == 0)) {
      filepath = argv[++i];
    }
    // verbose output
    if ((strcmp(argv[i], "-v") == 0) || (strcmp(argv[i], "--verbose") == 0)) {
      printf("[INFO]: verbose output enabled\n");
      //verbose = true;
    }
    // help
    if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
      printhelp();
      exit(EXIT_SUCCESS);
    }
  }
  
  // using files
  if (strcmp(filepath, "") != 0) {
    FILE* testfile = NULL;
    testfile = fopen(filepath, "r"); 
    if (testfile == NULL) {
      fprintf(stderr, "[ERR!]: failed to open file %s\n", filepath);
      exit(EXIT_FAILURE);
    }
    
    // locals for line processing
    char* line = "";
    U32 charsread = 0;
    size_t len = 0;
    U32 i = 1;
  
    // loop over each line
    while ((charsread = getline(&line, &len, testfile)) != -1) {
    
#ifdef DEBUG_LINES  // print the line as its read
      printf("[DBUG]: read \'%s\'\n", line);
#endif //DEBUG_LINES

      // sanitize input a little with strtoul endptr checking
      char* eptr = "\0";
  
      // get number for the prime
      U32 number = (U32)strtoull(strtok(line, "\n"), &eptr, 10);
      if ((strcmp(line, "\0") == 0) || !(strcmp(eptr, "\0") == 0)) {     // input sanity check
        fprintf(stderr, "[ERR!]: invalid digit(s) \'%s\' for arg \'number\'. exiting...\n", eptr);
        fclose(testfile);
        exit(EXIT_FAILURE);
      }
      
      // check the primality with miller-rabin and print results
      FLAG result = isprime(number, 5, verbosity);
#ifdef OUTPUT_ALL
      printf("#%d: ", i);
      printf("%d %s\n", number, result?"maybe prime":"not prime");
#endif
      if (!result) {
        STR msg = malloc(BUFF_SIZE);
        sprintf(msg, "%d not prime", number);
        print_pretty(INFO_MSG, __func__, msg, verbosity);
//#ifndef DEBUG // dont just run away on "release" build
        fclose(testfile);
        free(msg);
        exit(EXIT_FAILURE);
//#endif //DEBUG

      }
      i++;
    }
    
    fclose(testfile);
  }
  
  // using cli input
  else {
    // sanitize input a little with strtoul endptr checking
    char* eptr = "\0";
    
    // get number
    U32 number = (U32)strtoull(argv[1], &eptr, 10);
    if ((strcmp(argv[1], "\0") == 0) || !(strcmp(eptr, "\0") == 0)) {     // input sanity check
      fprintf(stderr, "[ERR!]: invalid digit(s) \'%s\' in arg 1 \'number\'. exiting...\n", eptr);
      exit(EXIT_FAILURE);
    } // get iterations
    U32 iterations = (U32)strtoull(argv[2], &eptr, 10);
    if ((strcmp(argv[2], "\0") == 0) || !(strcmp(eptr, "\0") == 0)) {     // input sanity check
      fprintf(stderr, "[ERR!]: invalid digit(s) \'%s\' in arg 2 \'iterations\'. exiting...\n", eptr);
      exit(EXIT_FAILURE);
    }
    
    // check the primality with miller-rabin and print results
    FLAG result = isprime(number, iterations, verbosity);
    printf("%d %s\n", number, result?"maybe prime":"not prime");

  
  }

  // back to os
  return 0;
}

#endif //PRIME_STANDALONE