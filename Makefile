all:
	cd src && make 
	@if [ -e src/_comrade ];then \
		if [ ! -d bin ]; then \
		mkdir bin; \
		fi; \
	    cp src/comrade.sh bin/comrade; \
		chmod +x bin/comrade; \
		mv src/_comrade bin/; \
	fi

clean:
	@if [ -e bin ]; then \
		rm bin -rf; \
		echo rm bin -rf; \
	fi
	@cd src && make clean

install:
	cp bin/comrade /usr/local/bin/comrade
	cp bin/_comrade /usr/local/bin/_comrade

	
