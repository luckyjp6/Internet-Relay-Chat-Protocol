objects = main.o functions.o print_msg.o
source = $(objects:.o=.cpp)

.PHONY: IRC
IRC: clean $(objects)
	g++ -o IRC $(objects)

$(objects): functions.h
	g++ -c $(source)

.PHONY: clean
clean:
	rm *.o