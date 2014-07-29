SRCS=GfxRip.cpp
OBJS=GfxRip.o
PROG=GfxRip
LIBS=bgui2/lib/libbgui.a

CXXFLAGS=-fpermissive

-include config.mak

all: $(PROG)

clean:
	rm -f $(PROG)
	rm -f $(OBJS)

$(PROG):  $(OBJS) $(LIBS)
	$(CXX) -o $@ $^ -lalleg $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(INC) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(LIBS):
	make -C bgui2/src -f Makefile.unx

.PHONY: all clean

