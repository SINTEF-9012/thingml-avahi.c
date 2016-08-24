PREFIX = /home/vassik/avahiinstall
INCLUDE_DIR = -I${PREFIX}/include
LIBS_DIR = -L${PREFIX}/lib

CROSS_COMPILE =

GCC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
LIBS = -lavahi-client -lavahi-common

GFLAGS = -O0 -g3 -Wall
#GFLAGS =

SRCS = ./src/thingml-avahi.c
OBJS = ./src/thingml-avahi.o

EXP_SRC = ./src/examples/thingml-avahi-example.c
EXP_OBJ = ./src/examples/thingml-avahi-example.o
EXP_BIN = ./src/examples/thingml-avahi-example

STATIC_LIB_LOCATION = ./src/libtmlavahi.a
DYNAMIC_LIB_LOCATION = ./src/libtmlavahi.so

LIB_THL_AHAVI = -ltmlavahi
LIB_THL_AHAVI_DIR = -L./src

%.o : %.c
	$(GCC) $(GFLAGS) ${INCLUDE_DIR} -c -o $@ $<

all : staticlib dynamiclib thingml-avahi-example-debug

thingml-avahi-example-debug: $(EXP_OBJ) $(OBJS)
	$(GCC) -o $(EXP_BIN) $(EXP_OBJ) $(OBJS) $(LIBS) ${LIBS_DIR}
	
thingml-avahi-example: $(EXP_OBJ) $(OBJS)
	$(GCC) -o $(EXP_BIN) $(EXP_OBJ) $(STATIC_LIB_LOCATION) $(LIBS) ${LIBS_DIR}

staticlib : $(OBJS)
	$(AR) crs $(STATIC_LIB_LOCATION) $(OBJS)

dynamiclib : $(OBJS)
	$(GCC) -shared -rdynamic -o $(DYNAMIC_LIB_LOCATION) $(OBJS)
	
clean:
	rm -rf *.o $(STATIC_LIB_LOCATION) $(DYNAMIC_LIB_LOCATION) $(EXP_OBJ) $(OBJS) $(EXP_BIN)
