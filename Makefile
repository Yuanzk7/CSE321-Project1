CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Iinclude
SRC = ./src
TARGET = btree

SRCS = $(wildcard $(SRC)/*.cpp)

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

.PHONY: clean
clean:
	rm -f $(TARGET)