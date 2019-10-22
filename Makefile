all: build clean

build: main.o FoxFires.o Stars.o RenderLayer.o Ground.o Sky.o Pine.o
	g++ main.o FoxFires.o Stars.o RenderLayer.o Ground.o Sky.o Pine.o -lsfml-graphics -lsfml-window -lsfml-system -o fox-fires

Ground.o: Ground.cpp
	g++ -c Ground.cpp
	
main.o: main.cpp
	g++ -c main.cpp
	
FoxFires.o: FoxFires.cpp
	g++ -c FoxFires.cpp
	
Stars.o: Stars.cpp
	g++ -c Stars.cpp
	
Sky.o: Sky.cpp
	g++ -c Sky.cpp
	
RenderLayer.o: RenderLayer.cpp
	g++ -c RenderLayer.cpp
	
Pine.o: Pine.cpp
	g++ -c Pine.cpp

clean:
	rm *.o
