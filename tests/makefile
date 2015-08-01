path.o: 
	cc -c ../path.c -o ./path.o
lock.o: 
	cc -c ../lock.c -o ./lock.o

locktest: path.o lock.o

# This only works on linux 64bit 
ns_test:
	gcc ns_test.c ../notification_sound.c ../lib/linux/x86_64/libgorilla.a -lm -lpthread ../lib/linux/x86_64/libopenal.so -o ns_test 

clean:
	rm *.o
	rm locktest
	rm ns_test

