prefix = /home/vassik/avahiinstall
includedir = -I${prefix}/include
libdir = -L${prefix}/lib

CC = gcc
LIBS = -lavahi-client -lavahi-common
#CFLAGS = -g -O2 -fstack-protector -std=c99 -Wall -W -Wextra -pedantic -pipe -Wformat -Wold-style-definition -Wdeclaration-after-statement -Wfloat-equal -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes -Wredundant-decls -Wmissing-noreturn -Wshadow -Wendif-labels -Wpointer-arith -Wbad-function-cast -Wcast-qual -Wcast-align -Wwrite-strings -fdiagnostics-show-option -Wno-cast-qual -fno-strict-aliasing

CFLAGS = -g -O2

SRCS = client-publish-service-threaded.c
OBJS = client-publish-service-threaded.o

SRCS1 = client-publish-service.c
OBJS1 = client-publish-service.o

all : client-publish-service-threaded client-publish-service
 
.c.o :
	${CC} ${CFLAGS} ${includedir} -c $< 

client-publish-service-threaded : $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS) ${libdir} -lm

client-publish-service : $(OBJS1)
	$(CC) -o $@ $(OBJS1) $(LIBS) ${libdir} -lm

clean:
	rm -f *.o *~ client-publish-service-threaded client-publish-service
