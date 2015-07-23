
cpp_files = $(shell find ./src -name '*.cpp')

include_dirs = -Iinclude

cflags = -g -std=c++0x

libs = -lOpenCL

all:
	@echo $(cpp_files)
	g++ $(cflags) main.cpp $(cpp_files) -o solv $(include_dirs) $(libs)

