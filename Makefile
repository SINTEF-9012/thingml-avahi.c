prefix = /home/vassik/avahiinstall
includedir = -I${prefix}/include
libdir = -L${prefix}/lib

CC = gcc
LIBS = -lavahi-client -lavahi-common
#CFLAGS = -g -O2 -fstack-protector -std=c99 -Wall -W -Wextra -pedantic -pipe -Wformat -Wold-style-definition -Wdeclaration-after-statement -Wfloat-equal -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes -Wredundant-decls -Wmissing-noreturn -Wshadow -Wendif-labels -Wpointer-arith -Wbad-function-cast -Wcast-qual -Wcast-align -Wwrite-strings -fdiagnostics-show-option -Wno-cast-qual -fno-strict-aliasing

CFLAGS = -g -O2

EXP_SRCS = ./src/examples/client-publish-service.c
EXP_OBJS = ./src/examples/client-publish-service.o
EXP_BIN = ./src/examples/client-publish-service

all : client-publish-service

.c.o :
	${CC} ${CFLAGS} ${includedir} -c $< 

client-publish-service : $(EXP_OBJS)
	$(CC) -o $@ $(EXP_OBJS) $(LIBS) ${libdir} -lm

clean:
	rm -f $(EXP_OBJS) $(EXP_BIN)
