CXX = g++
CXXFLAGS = -O3 -std=c++17 -pthread
TARGET = roulette
SRCS = roulette.cpp lib/CMap/cmap.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
