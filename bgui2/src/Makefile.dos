# Makefile for BGui 2.0

CC=gcc
LD=ld
CFLAGS=-O
FILES=../lib/libbgui.a btest

all: $(FILES)

btest: btest.c tdgui.h
	$(CC) $(CFLAGS) -o ../btest.exe btest.c ../lib/libbgui.a -lalleg

../lib/libbgui.a: bgfx.o bgui.o billfs.o bmenu.o bprocs.o
	ar rs ../lib/libbgui.a bgfx.o bgui.o billfs.o bmenu.o bprocs.o

bgfx.o: bgfx.c tdgui.h binterna.h
	$(CC) $(CFLAGS) -c bgfx.c

bgui.o: bgui.c tdgui.h binterna.h
	$(CC) $(CFLAGS) -c bgui.c

billfs.o: billfs.c tdgui.h binterna.h
	$(CC) $(CFLAGS) -c billfs.c

bmenu.o: bmenu.c tdgui.h binterna.h
	$(CC) $(CFLAGS) -c bmenu.c

bprocs.o: bprocs.c tdgui.h binterna.h
	$(CC) $(CFLAGS) -c bprocs.c

clean:
	rm -f *.o ../lib/libbgui.so ../lib/libbgui.a btest

