MUSL_VERSION=v1.2.1

CXXFLAGS=-I../.. -std=c++17 -Wall -fno-exceptions -nodefaultlibs -nostdlib -ggdb
LIBS=../../3rd_party/musl-${MUSL_VERSION}-dist/usr/local/musl/lib/crt*.o \
	 -L ../../3rd_party/musl-${MUSL_VERSION}-dist/usr/local/musl/lib/ \
	 -l:libc.a

.PHONY: run

run: ./hello
	./hello
	./hello --traditional
	ldd hello || true

../../3rd_party/musl-${MUSL_VERSION}-dist:
	(cd ../../3rd_party/ && ./build_musl.sh)

hello: hello.cpp ../../aids.hpp ../../3rd_party/musl-${MUSL_VERSION}-dist
	$(CXX) $(CXXFLAGS) hello.cpp ../../3rd_party/musl-${MUSL_VERSION}-dist/usr/local/musl/lib/crtn.o -o hello $(LIBS)

