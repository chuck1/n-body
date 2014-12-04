#LIBS = -lOpenCL
LIBS = -lOpenCL -lGL -lglut -lGLU
#LIBS = -l/usr/lib/x86_64-linux-gnu/libOpenCL.so

SRC = src/other.cpp src/step_pairs.cpp src/step_bodies.cpp src/kernel.cpp src/universe.cpp src/Frame.cpp

O_FILES = $(patsubst src/%.cpp, build/%.o, $(SRC))

C_FLAGS = -std=c++0x -g -Wall -Werror

all: play.out solv.out

play.out: $(O_FILES) build/glut.o
	@echo link $@
	@g++ -g -o $@ build/glut.o $(O_FILES) $(LIBS) -I. $(C_FLAGS)

solv.out: $(O_FILES) build/main.o
	@echo link $@
	@g++ -g -o $@ build/main.o $(O_FILES) $(LIBS) -I. -pthread $(C_FLAGS)

$(O_FILES) build/main.o build/glut.o: build/%.o: src/%.cpp body.h
	@echo build $@
	@mkdir -p $(dir $@)
	@g++ -g -c -o $@ $< -I. $(C_FLAGS)

clean:
	rm -rf build/

cleandata:
	rm -f data/*
	rm -f files_*

