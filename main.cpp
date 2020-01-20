#include "aids.hpp"

using namespace aids;

int main(int argc, char *argv[])
{
    if (argc < 2) {
        println(stderr, "Usage: ", argv[0], " <entry-file>");
        exit(1);
    }

    auto result = read_whole_file(argv[1]);
    if (result.is_error) {
        println(stderr, "Could not read file `", argv[1], "`: ", result.error);
        exit(1);
    }
    auto content = result.unwrap;
    defer(content.free());

    println(stdout, content);

    return 0;
}
