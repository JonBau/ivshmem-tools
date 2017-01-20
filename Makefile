
CC              := mpicc
LD              := mpicc
CFLAGS          := -Wall -Wextra -O3 $(CPUFREQFLAG) -g -lcrypto -lrt -lpthread -luuid

LDFLAGS         := -lm -lpthread -luuid
LIBS            := 
RM              := rm -f

SRCS        	:= $(wildcard *.c) $(wildcard ../misc/*.c)
OBJS        	:= $(patsubst %.c,%.o,$(SRCS))
BINS        	:= meta_test meta_reset

.PHONY: clean

all: $(BINS) 


meta_test: meta_test.o
	$(LD) $(LIBS) $(LDFLAGS) -o $@ $^


meta_reset: meta_reset.o
	$(LD) $(LIBS) $(LDFLAGS) -o $@ $^


%.o: %.c
	$(CC) $(CPPFLAGS) -c $(CFLAGS) -o $@ $<

clean:
	$(RM) $(OBJS)
	$(RM) $(BINS)

