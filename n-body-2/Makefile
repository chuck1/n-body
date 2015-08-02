
cpp_files = $(shell find ./src -name '*.cpp')

inc_files = $(shell find ./include -name '*.hpp')

o_files   = $(patsubst %.cpp, ./build/object/%.o, $(cpp_files))

include_dirs = -Iinclude -I.

cflags = -g -std=c++0x -DCPU=1

libs = -lOpenCL -lboost_program_options -lglut -lGL -lGLU

main_files = solv.cpp play.cpp

main_o_files   = $(patsubst %.cpp, build/object/%.o, $(main_files))

bin_files = build/solv build/play

all: $(bin_files)

help:
	@echo $(bin_files)
	@echo $(main_o_files)
	@echo $(main_files)


$(bin_files): build/%: build/object/%.o $(o_files)
	@echo building $@
	@g++ $(cflags) $< $(o_files) -o $@ $(include_dirs) $(libs)

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

