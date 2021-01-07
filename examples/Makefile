ifdef OS # windows nt (mingw of msys2)
CXXFLAGS=-I../ -std=c++17 -Wall -fno-exceptions -ggdb
LIBS=
else # linux
CXXFLAGS=-I../ -std=c++17 -Wall -fno-exceptions -nodefaultlibs -ggdb
LIBS=-lc
endif

all: cat gol sprintln utf8 hashmap custom_struct_as_hashmap_key

cat: cat.cpp ../aids.hpp
	$(CXX) $(CXXFLAGS) -o cat cat.cpp $(LIBS)

gol: gol.cpp ../aids.hpp
	$(CXX) $(CXXFLAGS) -o gol gol.cpp $(LIBS)

sprintln: sprintln.cpp ../aids.hpp
	$(CXX) $(CXXFLAGS) -o sprintln sprintln.cpp $(LIBS)

utf8: utf8.cpp ../aids.hpp
	$(CXX) $(CXXFLAGS) -o utf8 utf8.cpp $(LIBS)

hashmap: hashmap.cpp ../aids.hpp
	$(CXX) $(CXXFLAGS) -o hashmap hashmap.cpp $(LIBS)

custom_struct_as_hashmap_key: custom_struct_as_hashmap_key.cpp ../aids.hpp
	$(CXX) $(CXXFLAGS) -o custom_struct_as_hashmap_key custom_struct_as_hashmap_key.cpp $(LIBS)

