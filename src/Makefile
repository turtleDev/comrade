CFLAGS_M=-Wall -g `pkg-config --cflags gtk+-2.0 glib-2.0`
LIBS_M=`pkg-config --libs gtk+-2.0 glib-2.0` -lnotify -lSDL2 -lSDL2_mixer
OBJS=config.o lock.o notification.o notification_sound.o ping.o path.o $(LIB_JSMN)
CFLAGS= -Wall  -g -O0

all: $(OBJS) 
	$(CC) $(CFLAGS_M) $(OBJS)  main.c  -o _comrade $(LIBS_M) 

clean:
	rm -i -f _comrade 
	rm -i -f *.o

.PHONY: clean

notification.o:
	$(CC) $(CFLAGS_M) notification.c -c -o notification.o


