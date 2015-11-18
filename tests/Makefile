locktest: 
	gcc locktest.c ../src/path.c ../src/lock.c -o locktest

ns_test: 
	gcc ns_test.c ../src/notification_sound.c ../src/path.c -o ns_test -lSDL2 -lSDL_mixer 

config_test: libjsmn.a
	gcc config_test.c ../src/config.c ../lib/jsmn/libjsmn.a -o config_test

libjsmn.a:
	cd ../lib/jsmn && make

notification_test:
	gcc notification_test.c ../src/notification.c ../src/path.c  ../src/notification_sound.c  `pkg-config --cflags --libs gtk+-2.0 glib-2.0 ` -lSDL2 -lSDL2_mixer -lnotify -o notification_test

ping_test:
	gcc ping_test.c ../src/ping.c -o ping_test

clean:
	rm -i -f *.o
	rm -i -f locktest
	rm -i -f ns_test
	rm -i -f config_test
	rm -i -f notification_test
	rm -i -f ping_test
	cd ../lib/jsmn/ && make clean
