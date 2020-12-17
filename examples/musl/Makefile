CXXFLAGS=-I../.. -std=c++17 -Wall -fno-exceptions -nodefaultlibs -ggdb
LIBS=-lc

.PHONY: run

run: ./hello
	./hello
	./hello --traditional

hello: hello.cpp ../../aids.hpp
	$(CXX) $(CXXFLAGS) -o hello hello.cpp $(LIBS)

