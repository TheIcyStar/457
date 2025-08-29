CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = out
SRCDIR = HW1
SOURCES = $(SRCDIR)/main.cpp

.PHONY: all clean build

all: build

build: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
