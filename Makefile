SRCS=GfxRip.c
OBJS=GfxRip.o
PROG=GfxRip
LIBS=bgui2/lib/libbgui.a

CFLAGS=-std=c99

-include config.mak

all: $(PROG)

clean:
	rm -f $(PROG)
	rm -f $(OBJS)

$(PROG):  $(OBJS) $(LIBS)
	$(CC) -o $@ $^ -lalleg $(LDFLAGS)

%.o: %.cpp
	$(CC) $(INC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(LIBS):
	make -C bgui2/src -f Makefile.unx

.PHONY: all clean

