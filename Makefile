#108303553's PA4 cpp compilier
.PHONY: all run clean
all: 108303553_PA4.o
	@g++ -std=c++11 108303553_PA4.o -o exe
run: 
	@./exe $(input) $(output)
clean:
	@rm *.o
	@rm exe
108303553_PA4.o: 108303553_PA4.cpp
	@g++ -std=c++11 -c 108303553_PA4.cpp


