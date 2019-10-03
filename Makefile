all: build clean

build: main.o
	g++ main.o -lsfml-graphics -lsfml-window -lsfml-system -o fox-fires
	
main.o: main.cpp
	g++ -c main.cpp

clean:
	rm *.o
