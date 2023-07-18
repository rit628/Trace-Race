ifeq ($(OS), Windows_NT)
	EXECUTABLE := game.exe
else
	EXECUTABLE := game
endif

build:
	-g++ -std=c++14 src/*.cpp -o $(EXECUTABLE) -lsfml-window -lsfml-graphics -lsfml-system -lsfml-audio -lsfml-network

run:
	./game