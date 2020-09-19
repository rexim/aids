CXXFLAGS=-I../ -std=c++17 -Wall -fno-exceptions -nodefaultlibs -ggdb
LIBS=-lc

all: cat gol sprintln utf8

cat: cat.cpp ../aids.hpp
	$(CXX) $(CXXFLAGS) -o cat cat.cpp $(LIBS)

gol: gol.cpp ../aids.hpp
	$(CXX) $(CXXFLAGS) -o gol gol.cpp $(LIBS)

sprintln: sprintln.cpp ../aids.hpp
	$(CXX) $(CXXFLAGS) -o sprintln sprintln.cpp $(LIBS)

utf8: utf8.cpp ../aids.hpp
	$(CXX) $(CXXFLAGS) -o utf8 utf8.cpp $(LIBS)

