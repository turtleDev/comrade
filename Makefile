all:
	cd src && make 
	@if [ -e src/comrade ];then \
		if [ ! -d bin ]; then \
		mkdir bin; \
		fi; \
	    mv src/comrade bin/; \
	fi

	@echo "\n\nBUILD SUCESSFUL"
clean:
	@if [ -e bin ]; then \
		rm bin -rf; \
		echo rm bin -rf; \
	fi
	@cd src && make clean
