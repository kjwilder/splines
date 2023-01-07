# Basics
SHELL = /bin/sh

#.SUFFIXES:
#.SUFFIXES: .cc .o .h .d

# Indicate how to compile and link the program.
CC = g++ -std=c++11
CCINCS = -I/usr/local/include
#CCOPTS = -g -Wall -DUSE_ASSERTS -fno-implicit-templates
CCOPTS = -g -Wall -DUSE_ASSERTS -Wno-c++11-extensions
#CCOPTS = -O3 -Wc++11-extensions

LD = $(CC)
LDOPTS = 
LDLIBS = -lX11

# Here are the files used to make the program.
HEADERS = globlist.h globals.h grid.h includes.h image.h view.h spline.h
SOURCES = main.cc globals.cc view.cc image.cc spline.cc
OBJECTS = $(SOURCES:.cc=.o)

# Pattern rules
%.o : %.cc
	$(CC) $(CCINCS) $(CCOPTS) -c $<

%.d: %.cc
	$(SHELL) -ec '$(CC) -MM $< | sed '\''s/$*\\.o[ :]*/& $@/g'\'' > $@'

# Make the source
splines : $(HEADERS) $(OBJECTS) 
	$(LD) $(LDOPTS) $(OBJECTS) -o splines $(LDLIBS)

sources : $(SOURCES)

headers : $(HEADERS)

clean :
	rm -f *.o splines globals.h globals.cc

globals.h:
	curl -s https://raw.githubusercontent.com/kjwilder/globals/main/globals.h > globals.h

globals.cc:
	curl -s https://raw.githubusercontent.com/kjwilder/globals/main/globals.cc > globals.cc

