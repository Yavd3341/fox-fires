all: build clean

build: main.o FoxFires.o Stars.o RenderLayer.o Environment.o
	g++ main.o FoxFires.o Stars.o RenderLayer.o Environment.o -lsfml-graphics -lsfml-window -lsfml-system -o fox-fires

Environment.o: Environment.cpp
	g++ -c Environment.cpp
	
main.o: main.cpp
	g++ -c main.cpp
	
FoxFires.o: FoxFires.cpp
	g++ -c FoxFires.cpp
	
Stars.o: Stars.cpp
	g++ -c Stars.cpp
	
RenderLayer.o: RenderLayer.cpp
	g++ -c RenderLayer.cpp

clean:
	rm *.o
