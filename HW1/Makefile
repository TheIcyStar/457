CXX = g++
CXXFLAGS = -Wall -ansi -pedantic
CXXFLAGS += -g
TARGET = hw1
SOURCES =

SOURCES += ./hw1.cxx

.PHONY: all clean build

all: build

build: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
