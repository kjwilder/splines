SHELL = /bin/sh

CC = g++ -std=c++11 -Wall
LD = g++ -std=c++11 -Wall

X11_CFLAGS := $(shell pkg-config --cflags x11 2>/dev/null)
X11_LIBS := $(shell pkg-config --libs x11 2>/dev/null || echo "-lX11")

HEADERS = globlist.h globals.h grid.h includes.h image.h view.h spline.h spline_math.h
SOURCES = main.cc globals.cc view.cc image.cc spline.cc
OBJECTS = $(SOURCES:.cc=.o)

%.o : %.cc
	$(CC) $(X11_CFLAGS) $(CCINCS) -c $< -o $@

splines : $(HEADERS) $(OBJECTS)
	$(LD) $(X11_LIBS) $(LDOPTS) $(OBJECTS) -o splines

# Tests (no X11 dependency)
TEST_SOURCES = tests/test_main.cc tests/test_grid.cc tests/test_includes.cc tests/test_spline_math.cc
TEST_OBJECTS = $(patsubst tests/%.cc,tests/%.o,$(TEST_SOURCES))

tests/%.o : tests/%.cc tests/doctest.h grid.h includes.h spline_math.h
	$(CC) -DUSE_ASSERTS -Itests -I. -c $< -o $@

test_runner : $(TEST_OBJECTS)
	$(LD) $(TEST_OBJECTS) -o test_runner

test : test_runner
	./test_runner

clean :
	rm -f *.o tests/*.o splines test_runner globals.h globals.cc

globals.h:
	curl -s https://raw.githubusercontent.com/kjwilder/globals/main/globals.h > globals.h

globals.cc:
	curl -s https://raw.githubusercontent.com/kjwilder/globals/main/globals.cc > globals.cc
