#LIBS = -lOpenCL
LIBS = -lOpenCL -lGL -lglut -lGLU
#LIBS = -l/usr/lib/x86_64-linux-gnu/libOpenCL.so

SRC = $(shell find src -name '*.cpp')

O_FILES = $(patsubst src/%.cpp, build/%.o, $(SRC))
D_FILES = $(patsubst src/%.cpp, build/%.d, $(SRC))

C_FLAGS = -std=c++0x -g -Wall -Werror -pthread -MMD

INCLUDE_DIRS = -I. -I./include/


#all: play.out solv.out $(D_FILES)

all: play.out solv.out

help:
	@echo $(SRC)
	@echo $(O_FILES)

#$(D_FILES) build/main.d build/glut.d: build/%.d: src/%.cpp
#	@echo $< $@
#	@#g++ $(C_FLAGS) $(INCLUDE_DIRS) -MM -MT '$(patsubst src/%.cpp,build/%.o,$<)' $< -MF $@
#	@g++ $(C_FLAGS) $(INCLUDE_DIRS) -MM $< -MF $@

play.out: $(O_FILES) build/play.o
	@echo link $@
	@g++ -g -o $@ build/play.o $(O_FILES) $(LIBS) $(INCLUDE_DIRS) $(C_FLAGS)

solv.out: $(O_FILES) build/solv.o
	@echo link $@
	@g++ -g -o $@ build/solv.o $(O_FILES) $(LIBS) $(INCLUDE_DIRS) $(C_FLAGS)

$(O_FILES): build/%.o: src/%.cpp
	@echo build $@
	@mkdir -p $(dir $@)
	@g++ -g -c -o $@ $< -I. $(C_FLAGS) $(INCLUDE_DIRS)

build/play.o build/solv.o: build/%.o: %.cpp
	@echo build $@
	@mkdir -p $(dir $@)
	@g++ -g -c -o $@ $< -I. $(C_FLAGS) $(INCLUDE_DIRS)

clean:
	rm -rf build/

cleandata:
	rm -f data/*
	rm -f files_*

-include $(D_FILES)
-include build/solv.d
-include build/play.d


