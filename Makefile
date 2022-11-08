objects = functions.o print_msg.o error_func.o
source = $(objects:.o=.cpp)

IRC: main.o $(objects)
	g++ -o IRC main.o $(objects)

main.o : main.cpp
	g++ -c main.cpp

functions.o : functions.cpp functions.h
	g++ -c functions.cpp

print_msg.o : print_msg.cpp print_msg.h
	g++ -c print_msg.cpp

error_func.o : error_func.cpp error_func.h
	g++ -c error_func.cpp
 
.PHONY: clean
clean:
	-rm IRC $(objects)