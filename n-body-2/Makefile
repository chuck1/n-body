
cpp_files = $(shell find ./src -name '*.cpp')

include_dirs = -Iinclude -I.

cflags = -g -std=c++0x -DCPU=1

libs = -lOpenCL -lboost_program_options

all:
	@echo $(cpp_files)
	g++ $(cflags) main.cpp $(cpp_files) -o solv $(include_dirs) $(libs)

