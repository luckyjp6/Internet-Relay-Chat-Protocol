objects = functions.o print_msg.o error_func.o
source = $(objects:.o=.cpp)

IRC: main.o $(objects)
	g++ -o IRC main.o $(objects)

main.o : $(objects:.o=.h)
	g++ -c main.cpp

functions.o : 
	g++ -c functions.cpp

print_msg.o : name.h functions.h
	g++ -c print_msg.cpp

error_func.o : functions.h
	g++ -c error_func.cpp
 
.PHONY: clean
clean:
	-rm IRC $(objects)