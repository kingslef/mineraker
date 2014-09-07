CFLAGS=-std=c++11 -pedantic -Wall -Wextra
CFLAGS+=-Wformat=2 -Wswitch-default -Wcast-align -Wpointer-arith
CFLAGS+=-Winline -Wundef -Wcast-qual
CFLAGS+=-Wshadow -Wwrite-strings -Wstrict-aliasing=2 -Wunused-result
CFLAGS+=-mwindows
CFLAGS+=-DSFML_STATIC

INCDIR=-I/c/SFML-2.1/include -Ic:/SFML-2.1/include

CFLAGS+=$(INCDIR)
LIBDIR=-L/c/SFML-2.1/lib -Lc:\SFML-2.1/lib

LDLIBS=$(LIBDIR) -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lsfml-main

.PHONY: all

all: minerake

minerake: main.cpp boxField.cpp
	$(CXX) $^ $(CFLAGS) $(LDLIBS) -o $@

.PHONY: clean

clean:
	rm -f *.o *.a *.exe *~
