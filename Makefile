
TARGET=main
SOURCE=src
INCLUDE=include

CXXFLAGS:=$(CXXFLAGS) -std=c++11 -Wall -Wextra -Werror -I$(INCLUDE)

CXXSRC=$(shell find $(SOURCE) -type f -name '*.cpp')
CXXOBJ=$(CXXSRC:.cpp=.o)
CXXDEP=$(CXXSRC:.cpp=.d)

.PHONY: clean test

all: $(TARGET)

-include $(CXXDEP)

$(TARGET): $(CXXOBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(CXXOBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c -o $@ $<

clean:
	$(RM) $(TARGET)
	$(RM) $(CXXOBJ)
	$(RM) $(CXXDEP)

test: $(TARGET)
	echo "file clk/tests" | ./$(TARGET)
