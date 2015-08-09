CFLAGS=-Wall -g `pkg-config --cflags gtk+-2.0 glib-2.0`
LIBS=`pkg-config --libs gtk+-2.0 glib-2.0`
OBJS=config.o lock.o notification.o notification_sound.o ping.o path.o

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) main.c  -o comrade $(LIBS) 
clean:
	rm -i -f comrade 

