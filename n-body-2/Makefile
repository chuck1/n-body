
cpp_files = $(shell find ./src -name '*.cpp')

inc_files = $(shell find ./include -name '*.hpp')

o_files   = $(patsubst %.cpp, ./build/object/%.o, $(cpp_files))

include_dirs = -Iinclude -I.

cflags = -g -std=c++0x -DCPU=1

libs_solv = -lOpenCL -lboost_program_options
libs_play = -lboost_program_options -lglut -lGL -lGLU
libs_stat = -lboost_program_options

main_files = solv.cpp play.cpp stat.cpp

main_o_files   = $(patsubst %.cpp, build/object/%.o, $(main_files))

bin_files = build/solv build/play build/stat

all: $(bin_files)

o_files_play  = ./build/object/./src/Body.o ./build/object/./src/Frame.o ./build/object/./src/Universe.o
o_files_play += ./build/object/./src/Frames.o
o_files_play += ./build/object/./src/Branches.o
o_files_play += ./build/object/./src/Branch.o
o_files_play += ./build/object/./src/free.o

help:
	@echo $(bin_files)
	@echo $(main_o_files)
	@echo $(main_files)
	@echo $(o_files)


build/play: build/object/play.o $(o_files_play)
	@echo building $@
	@g++ $(cflags) $^ -o $@ $(include_dirs) $(libs_play)

build/stat: build/object/stat.o $(o_files_play)
	@echo building $@
	@g++ $(cflags) $^ -o $@ $(include_dirs) $(libs_stat)

build/solv: build/object/solv.o $(o_files)
	@echo building $@
	@g++ $(cflags) $^ -o $@ $(include_dirs) $(libs_solv)


$(main_o_files): build/object/%.o: %.cpp $(inc_files)
	@echo building $@
	@mkdir -p $(dir $@)
	@g++ -c $< -o $@ $(cflags) $(include_dirs)


$(o_files): build/object/%.o: %.cpp $(inc_files)
	@echo building $@
	@mkdir -p $(dir $@)
	@g++ -c $< -o $@ $(cflags) $(include_dirs)
	
cleandata:
	rm -f data/*
	rm -f files_*

clean:
	rm -rf build/

cleanvideo:
	rm -f capture/*

