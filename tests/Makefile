OS=$(shell uname -o)


locktest: 
	gcc -g locktest.c ../src/path.c ../src/lock.c -o locktest

ns_test: ns_test.c ../src/notification_sound.c ../src/path.c
ifeq ($(OS),Msys)
	gcc -g -I/usr/include -o ns_text ns_test.c ../src/notification_sound.c ../src/path.c -L/usr/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer
endif
ifeq ($(OS),GNU/Linux)
	gcc -g ns_test.c ../src/notification_sound.c ../src/path.c -o ns_test -lSDL2 -lSDL2_mixer 
endif

config_test: config_test.c
	gcc -g config_test.c ../src/config.c -o config_test

notification_test:
	gcc -g notification_test.c ../src/notification.c ../src/path.c  ../src/notification_sound.c  `pkg-config --cflags --libs gtk+-2.0 glib-2.0 ` -lSDL2 -lSDL2_mixer -lnotify -o notification_test

ping_test:
	gcc -g ping_test.c ../src/ping.c -o ping_test

clean:
	rm -i -f *.o *.exe
	rm -i -f locktest
	rm -i -f ns_test
	rm -i -f config_test
	rm -i -f notification_test
	rm -i -f ping_test
