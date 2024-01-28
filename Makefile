CXX=g++

OUT=clack

CXXFLAGS=-O2 -std=c++11 -Iinclude/

CPPSRC=$(wildcard src/*.cpp) $(wildcard *.cpp)

all: $(OUT)

$(OUT): $(CPPSRC)
	$(CXX) -o $(OUT) $(CPPSRC) $(CXXFLAGS)

run: $(OUT)
	./$(OUT)

clean:
	rm $(OUT)

test: $(compile)
	echo "file clk/tests" | ./$(OUT)
