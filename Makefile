
CC              := mpicc
LD              := mpicc
CFLAGS          := -Wall -Wextra -O3 $(CPUFREQFLAG) -g -lcrypto -lrt -lpthread

LDFLAGS         := -lm
LIBS            := 
RM              := rm -f

SRCS        	:= $(wildcard *.c) $(wildcard ../misc/*.c)
OBJS        	:= $(patsubst %.c,%.o,$(SRCS))
BINS        	:= meta_test

.PHONY: clean

all: $(BINS) 


meta_test: meta_test.o
	$(LD) $(LIBS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CPPFLAGS) -c $(CFLAGS) -o $@ $<

clean:
	$(RM) $(OBJS)
	$(RM) $(BINS)

