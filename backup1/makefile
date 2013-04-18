EXECUTABLES = mmu

CC = gcc
CFLAGS = -O2

all: $(EXECUTABLES)

.SUFFIXES: .c .o
.c.o: 
	$(CC) $(CFLAGS) -c $*.c

mmu : mmu.o parser.o
	$(CC) $(CFLAGS) -o mmu mmu.o

clean: 
	rm -f core*.o $(EXECUTABLES)
