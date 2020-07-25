CXXFLAGS=-I../ -std=c++17 -Wall -fno-exceptions -ggdb

all: cat gol

cat: cat.cpp ../aids.hpp
	$(CXX) $(CXXFLAGS) -o cat cat.cpp

gol: gol.cpp ../aids.hpp
	$(CXX) $(CXXFLAGS) -o gol gol.cpp
