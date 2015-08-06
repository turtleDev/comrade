main:
	gcc -Wall -g config.c main.c notification.c notification_sound.c ping.c path.c -o main  ../lib/linux/x86_64/libjsmn.a ../lib/linux/x86_64/libgorilla.a -lm -lpthread -lnotify `pkg-config --libs --cflags glib-2.0 gtk+-2.0` ../lib/linux/x86_64/libopenal.so 

clean:
	rm -i -f  main

