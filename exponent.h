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
// exponent.h:
//  interface for the fast exponentiation algorithm
//


// header guard
#ifndef _EXPONENT_H_
#define _EXPONENT_H_

#include "util.h"


// does a mod exponent using fast exponentiation of format: n^e mod m
U32 fast_mod_exp(U32 n, U32 e, U32 m, BF8 verbosity);

#endif //_EXPONENT_H_
