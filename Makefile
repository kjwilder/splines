SHELL = /bin/sh

CC = g++ -std=c++11 -Wall
LD = g++ -std=c++11 -Wall
LDLIBS = -lX11

HEADERS = globlist.h globals.h grid.h includes.h image.h view.h spline.h
SOURCES = main.cc globals.cc view.cc image.cc spline.cc
OBJECTS = $(SOURCES:.cc=.o)

%.o : %.cc
	$(CC) $(CCINCS) -c $<

splines : $(HEADERS) $(OBJECTS) 
	$(LD) $(LDOPTS) $(OBJECTS) -o splines $(LDLIBS)

clean :
	rm -f *.o splines globals.h globals.cc

globals.h:
	curl -s https://raw.githubusercontent.com/kjwilder/globals/main/globals.h > globals.h

globals.cc:
	curl -s https://raw.githubusercontent.com/kjwilder/globals/main/globals.cc > globals.cc
