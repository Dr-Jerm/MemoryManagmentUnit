EXECUTABLES = mmu

CC = gcc
LIBS = libfdr.a
CFLAGS = -O2 -g

OBJS = mmusim.o parser.o util.o TLBandPGTBL.o
#all: $(EXECUTABLES)

#.SUFFIXES: .c .o
#.c.o: 
#	$(CC) $(CFLAGS) -c $*.c

mmu : memStruct.h $(OBJS) mmu.c
	gcc -g -o mmu mmu.c -lm

#parser.o : memStruct.h

#mmusim.o : memStruct.h


clean: 
	rm -f *.o $(EXECUTABLES)
