# Basics
SHELL = /bin/sh

#.SUFFIXES:
#.SUFFIXES: .C .o .h .d

# Indicate how to compile and link the program.
CC = g++ 
CCINCS = -I/usr/local/include
#CCOPTS = -g -Wall -DUSE_ASSERTS -fno-implicit-templates
CCOPTS = -g -Wall -DUSE_ASSERTS -Wno-c++11-extensions
#CCOPTS = -O3 -Wc++11-extensions

LD = $(CC)
LDOPTS = 
LDLIBS = -lX11

# Here are the files used to make the program.
HEADERS = globlist.h globals.h grid.h includes.h image.h view.h spline.h
SOURCES = main.C globals.C view.C image.C spline.C
OBJECTS = $(SOURCES:.C=.o)

# Pattern rules
%.o : %.C
	$(CC) $(CCINCS) $(CCOPTS) -c $<

%.d: %.C
	$(SHELL) -ec '$(CC) -MM $< | sed '\''s/$*\\.o[ :]*/& $@/g'\'' > $@'

# Make the source
splines : $(HEADERS) $(OBJECTS) 
	$(LD) $(LDOPTS) $(OBJECTS) -o splines $(LDLIBS)

make_mat : $(MAKE_MAT_OBJECTS)
	$(LD) $(LDOPTS) $(MAKE_MAT_OBJECTS) -o make_mat $(LDLIBS)

sources : $(SOURCES)

headers : $(HEADERS)

# Make tags files for use with vi and emacs.
tags: $(HEADERS) $(SOURCES)
	etags --c++ $(HEADERS) $(SOURCES) 
	ctags $(HEADERS) $(SOURCES) 
 
symbol:
	rcs  -n$(SYMBOL): Makefile $(HEADERS) $(SOURCES)

clean :
	rm -f *.o digits

ifndef NDF
 include $(SOURCES:.C=.d)
endif

