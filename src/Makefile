CFLAGS_M=-Wall -g `pkg-config --cflags gtk+-2.0 glib-2.0`
LIBS_M=`pkg-config --libs gtk+-2.0 glib-2.0` -lnotify -lpthread -lm -lopenal
OBJS=config.o lock.o notification.o notification_sound.o ping.o path.o $(LIBS)/*
LIBS=../lib/linux/x86_64
CFLAGS= -g -Wall

all: $(OBJS)
	$(CC) $(CFLAGS_M) $(OBJS)  main.c  -o comrade $(LIBS_M) 
clean:
	rm -i -f comrade 
	rm -i -f *.o

notification.o:
	$(CC) $(CFLAGS_M) notification.c -c -o notification.o
