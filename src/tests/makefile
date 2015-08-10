locktest: 
	gcc locktest.c ../path.c ../lock.c -o locktest

# This only works on linux 64bit 
ns_test: 
	gcc ns_test.c ../notification_sound.c ../path.c ../../lib/linux/x86_64/libgorilla.a -lm -lpthread -lopenal -o ns_test 

config_test:
	gcc config_test.c ../config.c ../../lib/linux/x86_64/libjsmn.a -o config_test

notification_test:
	gcc notification_test.c ../notification.c ../path.c  ../notification_sound.c  `pkg-config --cflags --libs gtk+-2.0 glib-2.0 ` ../../lib/linux/x86_64/libgorilla.a -lm -lnotify -lopenal -o notification_test

ping_test:
	gcc ping_test.c ../ping.c -o ping_test

clean:
	rm -i -f *.o
	rm -i -f locktest
	rm -i -f ns_test
	rm -i -f config_test
	rm -i -f notification_test
	rm -i -f ping_test
