#include "aids.hpp"

using namespace aids;

int main(int argc, char *argv[])
{
    if (argc < 2) {
        println(stderr, "Usage: ", argv[0], " <entry-file>");
        exit(1);
    }

    auto result = read_file_as_string_view(argv[1]);
    if (!result.has_value) {
        println(stderr, "Could not read file `", argv[1], "`: ", strerror(errno));
        exit(1);
    }
    auto content = result.unwrap;
    defer(free((void*) content.data));

    println(stdout, content);

    return 0;
}
