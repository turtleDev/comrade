CFLAGS_M=-Wall -g `pkg-config --cflags gtk+-2.0 glib-2.0`
LIBS_M=`pkg-config --libs gtk+-2.0 glib-2.0` -lnotify -lSDL2 -lSDL2_mixer
LIB_JSMN=../lib/jsmn/libjsmn.a
OBJS=config.o lock.o notification.o notification_sound.o ping.o path.o $(LIB_JSMN)
CFLAGS= -Wall -O0

all: libjsmn.a $(OBJS) 
	$(CC) $(CFLAGS_M) $(OBJS)  main.c  -o comrade $(LIBS_M) 

libjsmn.a: 
	@cd ../lib/jsmn/ && make

dev: $(OBJS)
	$(CC) $(CFLAGS_M) $(OBJS)  main.c  -o comrade $(LIBS_M) 

clean:
	rm -i -f comrade 
	rm -i -f *.o
	cd ../lib/jsmn && make clean

notification.o:
	$(CC) $(CFLAGS_M) notification.c -c -o notification.o


