prefix = /home/vassik/avahiinstall
includedir = -I${prefix}/include
libdir = -L${prefix}/lib

CROSS_COMPILE =

GCC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
LIBS = -lavahi-client -lavahi-common

CFLAGS = -g -O2

SRCS = ./src/thingml-avahi.c
OBJS = ./src/thingml-avahi.o

STATIC_LIB_LOCATION = ./src/libtmlavahi.a
DYNAMIC_LIB_LOCATION = ./src/libtmlavahi.so

.o : .c
	$(GCC) $(GFLAGS) ${includedir} -c -o $@ $<

all : staticlib dynamiclib

staticlib : $(OBJS)
	$(AR) crs $(STATIC_LIB_LOCATION) $(OBJS)

dynamiclib : $(OBJS)
	$(GCC) -shared -rdynamic -o $(DYNAMIC_LIB_LOCATION) $(OBJS)
	
clean:
	rm -rf *.o $(STATIC_LIB_LOCATION) $(DYNAMIC_LIB_LOCATION)
