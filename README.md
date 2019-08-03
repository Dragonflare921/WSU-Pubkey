WSU-Pubkey
==========

## Provides two small programs from HW4:

### exponent:  
&nbsp;&nbsp;Performs fast exponentiation mod n

### prime:  
&nbsp;&nbsp;Performs the miller-rabin algorithm with user specified repetitions

  
## Along with the pubkey program for P2:

### pubkey:  
&nbsp;&nbsp;Public key cryptosystem that supports keygen, encrypt, and decrypt modes
  

## Contents:

  - <span>README</span>:                  this file
  - <span>Makefile</span>:                build instructions
  - <span>exponent.c</span>:              implementation of fast exponentiation algorithm
  - <span>exponent.h</span>:              interface for fast exponentiation algorithm
  - <span>prime.c</span>:                 implementation of miller-rabin algorithm
  - <span>prime.h</span>:                 interface for miller-rabin algorithm
  - <span>pubkey.c</span>:                implementation of the public key cryptosystem
  - <span>pubkey.h</span>:                interface for the public key cryptosystem
  - <span>util.h</span>:                  utility definitions, macros, and function prototypes
  - <span>util.c</span>:                  utility function implementations

  
## Building:

to build all programs, just run
```
  $ make
```

for just exponent, run
```
  $ make exponent
```

for just prime, run
```
  $ make prime
```

for just pubkey, run
```
  $ make pubkey
```

  
the included makefile also supports "cleanup" rules
```
  $ make clean
  $ make clean_objs
  $ make clean_bins
```

## Usage:

### exponent:
```
  $ ./exponent -h
```

### miller_rabin:
```
  $ ./prime -h
```
  
### pubkey:
```
  $ ./pubkey -h
```
  

## Notes:
  you can skip digging into the nitty gritty details of things like util
  
  some of the scaffolding and supporting code may seem overkill, especially
  for an assignment of this scope, but a lot of it is tooling that im
  fleshing out to be *very* useful to reuse for the forseeable future.
  
  im also trying to develop better, safer, habits, especially with C, so i
  stop blowing my entire leg off when setting up new projects and low level
  systems code
  
  ### Verbosity level:  
  included in this project is a custom utility "library" which provides
  a printf wrapper/helper, allowing convenient verbosity options and
  message tagging to aid in logging messages of different types

  the verbosity option -v or --verbosity expects a bitfield expressed
  as an 8 bit hexadecimal mask of standard notation (prefixed by 0x)
    
  these masks are defined in util.h, and are provided below:
  ```
    P_SILENT    0x01
    P_ERROR     0x02
    P_WARN      0x04
    P_INFO      0x08
    P_DEBUG     0x10
    P_ASSERT    0x20
    P_EVENT     0x40
    P_ALERT     0x80
  ```

  by default, the verbosity is set to 0x0E, which is the equivalent of
  ```
    P_ERROR | P_WARN | P_INFO
  ```
  
  this is the typical use case and provides most of the useful
  runtime information needed to understand the operation of pubkey

  some message classes are unused in pubkey as they aren't relevant
  for this project, but are used in other codebases
    