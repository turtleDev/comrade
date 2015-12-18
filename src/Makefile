#  Comrade

# detect the OS
OS=$(shell uname -o)

# set global CFLAGS
CFLAGS= -Wall  -g -O0

# set CFLAGS for main module, depending upon OS
ifeq($(OS),Linux)
CFLAGS_MAIN=$(CFLAGS) `pkg-config --cflags gtk+-2.0 glib-2.0`
endif

ifeq($(OS),Cygwin)
CFLAGS_MAIN=$(CFLAGS)
endif

# set LIBS_MAIN depending upon the OS
ifeq($(OS),Linux)
LIBS_MAIN=`pkg-config --libs gtk+-2.0 glib-2.0` -lnotify -lSDL2 -lSDL2_mixer
endif

ifeq($(OS),Cygwin)
LIBS_MAIN=-lSDL2 -lSDL2_mixer
endif

OBJS=config.o lock.o notification.o notification_sound.o ping.o path.o 


all: $(OBJS) 
	$(CC) $(CFLAGS_MAIN) $(OBJS)  main.c  -o _comrade $(LIBS_MAIN) 

clean:
	rm -i -f _comrade 
	rm -i -f *.o

.PHONY: clean

notification.o:
	$(CC) $(CFLAGS_MAIN) notification.c -c -o notification.o


