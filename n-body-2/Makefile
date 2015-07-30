
cpp_files = $(shell find ./src -name '*.cpp')

inc_files = $(shell find ./include -name '*.hpp')

o_files   = $(patsubst ./src/%.cpp, ./build/object/%.o, $(cpp_files))

include_dirs = -Iinclude -I.

cflags = -g -std=c++0x -DCPU=1

libs = -lOpenCL -lboost_program_options -lglut -lGL -lGLU



all: $(o_files)
	@echo $(cpp_files)
	@mkdir -p build
	g++ $(cflags) main.cpp $(o_files) -o solv $(include_dirs) $(libs)
	g++ $(cflags) play.cpp $(o_files) -o build/play $(include_dirs) $(libs)

$(o_files): build/object/%.o: src/%.cpp $(inc_files)
	@mkdir -p $(dir $@)
	g++ -c $< -o $@ $(cflags) $(include_dirs)
	
cleandata:
	rm -f data/*
	rm -f files_*

