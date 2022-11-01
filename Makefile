CPPC=g++

OUT=clack

CPPFLAGS=-Os -std=c++11 -Iinclude/

CPPSRC=$(wildcard src/*.cpp) $(wildcard *.cpp)

all: compile

compile:
	$(CPPC) -o $(OUT) $(CPPSRC) $(CPPFLAGS)

run: $(OUT)
	./$(OUT)