#include "aids.hpp"

using namespace aids;

int main(int argc, char *argv[])
{
    if (argc < 2) {
        println(stderr, "Usage: ", argv[0], " <files...>");
        exit(1);
    }

    for (int i = 1; i < argc; ++i) {
        auto result = read_file_as_string_view(argv[i]);
        if (!result.has_value) {
            println(stderr, "Could not read file `", argv[i], "`: ", strerror(errno));
            exit(1);
        }
        auto content = result.unwrap;
        defer(destroy(content));

        println(stdout, content);
    }

    return 0;
}
