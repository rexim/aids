CXXFLAGS=-I../ -std=c++17 -Wall -fno-exceptions -nodefaultlibs -ggdb
LIBS=-lc

.PHONY: test
test: utf8_test hash_map_test
	./utf8_test
	./hash_map_test

utf8_test: utf8_test.cpp ../aids.hpp
	$(CXX) $(CXXFLAGS) -o utf8_test utf8_test.cpp $(LIBS)

hash_map_test: hash_map_test.cpp ../aids.hpp
	$(CXX) $(CXXFLAGS) -o hash_map_test hash_map_test.cpp $(LIBS)

