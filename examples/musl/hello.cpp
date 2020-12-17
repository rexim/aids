#include "aids.hpp"

int main(int argc, char *argv[]) {
    if (argc >= 2 && (0 == strcmp(argv[1], "--traditional"))) {
        aids::println(stdout, "hello, world");
    } else {
        aids::println(stdout, "Hello, world!");
    }
}

