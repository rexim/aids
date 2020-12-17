#include "aids.hpp"

int main(int argc, char *argv[]) {
    auto args = aids::Args{argc, argv};
    args.shift();               // skip program

    if (!args.empty() && !strcmp(args.shift(), "--traditional")) {
        aids::println(stdout, "hello, world");
    } else {
        aids::println(stdout, "Hello, world!");
    }
}
