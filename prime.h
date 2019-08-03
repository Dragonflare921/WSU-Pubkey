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
// prime.h:
//  interface for the miller rabin algorithm
//


// header guard
#ifndef _PRIME_H_
#define _PRIME_H_

#include "util.h"

// miller-rabin primality helper
FLAG witness(U64 a, U64 u, BF8 verbosity);

// takes a number n and iterations i and returns false if
// n is not prime, but returning true does not guarantee primality
FLAG isprime(U64 n, U32 i, BF8 verbosity);

#endif //_PRIME_H_
