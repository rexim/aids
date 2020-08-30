CXXFLAGS=-I../ -std=c++17 -Wall -fno-exceptions -nodefaultlibs -ggdb
LIBS=-lc

all: cat gol sprintln

cat: cat.cpp ../aids.hpp
	$(CXX) $(CXXFLAGS) -o cat cat.cpp $(LIBS)

gol: gol.cpp ../aids.hpp
	$(CXX) $(CXXFLAGS) -o gol gol.cpp $(LIBS)

sprintln: sprintln.cpp ../aids.hpp
	$(CXX) $(CXXFLAGS) -o sprintln sprintln.cpp $(LIBS)
