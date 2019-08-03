# playing with makefiles *even more*
# maybe some day i wont suck


# compiler flags
CC = gcc
CFLAGS = --std=c99 -Wall --pedantic -DQUICK_DISCARD
DFLAGS = -DDEBUG -DENC_DEBUG -DDEC_DEBUG

# libraries to be linked
CLIBS = -lm

# project source
UTIL_SRC = util.c
EXP_SRC = exponent.c
PRIME_SRC = prime.c
PUB_SRC = pubkey.c
SRC = $(UTIL_SRC) $(EXP_SRC) $(PRIME_SRC) $(PUB_SRC)

# project headers
UTIL_HDRS = util.h
EXP_HDRS = exponent.h
PRIME_HDRS = prime.h
PUB_HDRS = pubkey.h
HDRS = $(UTIL_HDRS) $(EXP_HDRS) $(PRIME_HDRS) $(PUB_HDRS)

# release objects
UTIL_OBJS = util.o
EXP_OBJS = exponent.o $(UTIL_OBJS)
PRIME_OBJS = prime.o $(EXP_OBJS)
PUB_OBJS = pubkey.o $(PRIME_OBJS)

# debug objects
D_UTIL_OBJS = d_util.o
D_EXP_OBJS = d_exponent.o $(D_UTIL_OBJS)
D_PRIME_OBJS = d_prime.o $(D_EXP_OBJS)
D_PUB_OBJS = d_pubkey.o $(D_PRIME_OBJS)

# standalone objects
#SA_UTIL_OBJS = sa_util.o
SA_EXP_OBJS = sa_exponent.o $(UTIL_OBJS)
SA_PRIME_OBJS = sa_prime.o $(EXP_OBJS)
D_SA_EXP_OBJS = d_sa_exponent.o $(D_UTIL_OBJS)
D_SA_PRIME_OBJS = d_sa_prime.o $(D_EXP_OBJS)


# TODO: these contain a bunch of dupes
#       not sure how much of a problem that is
# full object lists
OBJS = $(UTIL_OBJS) $(EXP_OBJS) $(PRIME_OBJS) $(PUB_OBJS)
SA_OBJS = $(UTIL_OBJS) $(SA_EXP_OBJS) $(SA_PRIME_OBJS)
D_OBJS = $(D_UTIL_OBJS) $(D_EXP_OBJS) $(D_PRIME_OBJS) $(D_PUB_OBJS)
D_SA_OBJS = $(D_UTIL_OBJS) $(D_SA_EXP_OBJS) $(D_SA_PRIME_OBJS)

# output binaries
EXP_BIN = exponent
PRIME_BIN = prime
PUB_BIN = pubkey
D_EXP_BIN = d_exponent
D_PRIME_BIN = d_prime
D_PUB_BIN = d_pubkey
BINS = $(EXP_BIN) $(PRIME_BIN) $(PUB_BIN)
D_BINS = $(D_EXP_BIN) $(D_PRIME_BIN) $(D_PUB_BIN)




# build rules

# make everything
all: $(BINS)

# make all debug builds
debug: $(D_BINS)




# build exponent as a standalone binary
$(EXP_BIN): $(SA_EXP_OBJS)
	$(CC) $(SA_EXP_OBJS) $(CLIBS) -o $(EXP_BIN)

# build exponent in debug mode as a standalone binary
$(D_EXP_BIN): $(D_SA_EXP_OBJS)
	$(CC) $(D_SA_EXP_OBJS) $(CLIBS) -o $(D_EXP_BIN)
	
	
	
	
# build prime as a standalone binary
$(PRIME_BIN): $(SA_PRIME_OBJS)
	$(CC) $(SA_PRIME_OBJS) $(CLIBS) -o $(PRIME_BIN)

# build prime in debug mode as a standalone binary
$(D_PRIME_BIN): $(D_SA_PRIME_OBJS)
	$(CC) $(D_SA_PRIME_OBJS) $(CLIBS) -o $(D_PRIME_BIN)

	
	

# build pubkey binary
$(PUB_BIN): $(PUB_OBJS)
	$(CC) $(PUB_OBJS) $(CLIBS) -o $(PUB_BIN)
	
# build pubkey binary in debug mode
$(D_PUB_BIN): $(D_PUB_OBJS)
	$(CC) $(D_PUB_OBJS) $(CLIBS) -o $(D_PUB_BIN)
	
	
# TODO: dont hardcode the object names

# build util object
util.o: $(UTIL_SRC) $(UTIL_HDRS)
	$(CC) -c $(CFLAGS) $(UTIL_SRC) -o util.o

# build exponent object
exponent.o: $(EXP_SRC) $(EXP_HDRS)
	$(CC) -c $(CFLAGS) $(EXP_SRC) -o exponent.o

# build prime object
prime.o: $(PRIME_SRC) $(PRIME_HDRS)
	$(CC) -c $(CFLAGS) $(PRIME_SRC) -o prime.o
	
# build pubkey object
pubkey.o: $(PUB_SRC) $(PUB_HDRS)
	$(CC) -c $(CFLAGS) $(PUB_SRC) -o pubkey.o

	
	
	
# build debug util object
d_util.o: $(UTIL_SRC) $(UTIL_HDRS)
	$(CC) -c $(CFLAGS) $(DFLAGS) $(UTIL_SRC) -o d_util.o

# build debug exponent object
d_exponent.o: $(EXP_SRC) $(EXP_HDRS)
	$(CC) -c $(CFLAGS) $(DFLAGS) $(EXP_SRC) -o d_exponent.o

# build debug prime object
d_prime.o: $(PRIME_SRC) $(PRIME_HDRS)
	$(CC) -c $(CFLAGS) $(DFLAGS) $(PRIME_SRC) -o d_prime.o
	
# build pubkey object
d_pubkey.o: $(PUB_SRC) $(PUB_HDRS)
	$(CC) -c $(CFLAGS) $(DFLAGS) $(PUB_SRC) -o d_pubkey.o

	
	
# build standalone exponent object
sa_exponent.o: $(EXP_SRC) $(EXP_HDRS)
	$(CC) -c $(CFLAGS) -DEXP_STANDALONE $(EXP_SRC) -o sa_exponent.o

# build standalone debug exponent object
d_sa_exponent.o: $(EXP_SRC) $(EXP_HDRS)
	$(CC) -c $(CFLAGS) $(DFLAGS) -DEXP_STANDALONE $(EXP_SRC) -o d_sa_exponent.o

	
	
	
# build standalone prime object
sa_prime.o: $(PRIME_SRC) $(PRIME_HDRS)
	$(CC) -c $(CFLAGS) -DPRIME_STANDALONE $(PRIME_SRC) -o sa_prime.o

# build standalone debug exponent object
d_sa_prime.o: $(PRIME_SRC) $(PRIME_HDRS)
	$(CC) -c $(CFLAGS) $(DFLAGS) -DPRIME_STANDALONE $(PRIME_SRC) -o d_sa_prime.o
	
	
	
# wrap everything into a tarball
tarball: $(SRC) $(HDRS) Makefile README
	tar -czvf bwarner_p2_pubkey.tar.gz ./*

	
	
	
.PHONY: clean
.PHONY: clean_objs
.PHONY: clean_bins

# TODO: only delete shit that actually exists
#       so bash doesnt yell every time things get cleaned
clean:
	rm $(BINS) $(D_BINS) $(OBJS) $(D_OBJS) $(SA_OBJS) $(D_SA_OBJS)

clean_objs:
	rm $(OBJS) $(D_OBJS) $(SA_OBJS) $(D_SA_OBJS)
	
clean_bins:
	rm $(BINS) $(D_BINS)

