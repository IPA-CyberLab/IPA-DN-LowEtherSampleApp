# Makefile

OPTIONS_COMPILE_DEBUG=-D_DEBUG -DDEBUG -D_REENTRANT -DREENTRANT -D_THREAD_SAFE -D_THREADSAFE -DTHREAD_SAFE -DTHREADSAFE -D_FILE_OFFSET_BITS=64 -I./nativelib/nativelib_src/ -g -fsigned-char

OPTIONS_LINK_DEBUG=-g -fsigned-char -lm -ldl -lrt -lpthread -lssl -lcrypto -lreadline -lncurses -lz

OPTIONS_COMPILE_RELEASE=-DNDEBUG -DVPN_SPEED -D_REENTRANT -DREENTRANT -D_THREAD_SAFE -D_THREADSAFE -DTHREAD_SAFE -DTHREADSAFE -D_FILE_OFFSET_BITS=64 -I./nativelib/nativelib_src/ -O2 -fsigned-char

OPTIONS_LINK_RELEASE=-O2 -fsigned-char -lm -ldl -lrt -lpthread -lssl -lcrypto -lreadline -lncurses -lz

HEADERS_NATIVELIB=nativelib/nativelib_src/nativelib.h

OBJECTS_NATIVELIB=obj/obj/linux/nativelib.o

ifeq ($(DEBUG),YES)
	OPTIONS_COMPILE=$(OPTIONS_COMPILE_DEBUG)
	OPTIONS_LINK=$(OPTIONS_LINK_DEBUG)
else
	OPTIONS_COMPILE=$(OPTIONS_COMPILE_RELEASE)
	OPTIONS_LINK=$(OPTIONS_LINK_RELEASE)
endif

HEADERS=$(wildcard *.h)
SRCS=$(wildcard *.c)
OBJS=$(addprefix obj/obj/linux/,$(patsubst %.c,%.o,$(SRCS)))


# Build Action
default:	build

build:	$(OBJECTS_NATIVELIB) bin/nativeapp

obj/obj/linux/nativelib.o: nativelib/nativelib_src/nativelib.c $(HEADERS_NATIVELIB)
	@mkdir -p obj/obj/linux/
	@mkdir -p bin/
	$(CC) $(OPTIONS_COMPILE) -c nativelib/nativelib_src/nativelib.c -o obj/obj/linux/nativelib.o

obj/obj/linux/%.o: %.c
	$(CC) $(OPTIONS_COMPILE) -c $< -o $@

bin/nativeapp: obj/obj/linux/nativelib.o $(HEADERS_NATIVELIB) $(OBJECTS_NATIVELIB) $(OBJS)
	$(CC) obj/obj/linux/nativelib.o $(OBJS) $(OPTIONS_LINK) -o bin/nativeapp

clean:
	-rm -f obj/obj/linux/*.o
	-rm -f bin/nativeapp

help:
	@echo "make [DEBUG=YES]"
	@echo "make install"
	@echo "make clean"


