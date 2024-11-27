CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++17
LDFLAGS = -lm -lsimlib

SRC = main.cpp
OUT = simulation

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(SRC) $(LDFLAGS)

clean:
	rm -f $(OUT)
