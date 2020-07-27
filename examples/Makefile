CXXFLAGS=-I../ -std=c++17 -Wall -fno-exceptions -nodefaultlibs -ggdb
LIBS=-lc

all: cat gol

cat: cat.cpp ../aids.hpp
	$(CXX) $(CXXFLAGS) -o cat cat.cpp $(LIBS)

gol: gol.cpp ../aids.hpp
	$(CXX) $(CXXFLAGS) -o gol gol.cpp $(LIBS)
