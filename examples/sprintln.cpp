#define AIDS_IMPLEMENTATION
#include "aids.hpp"

char buffer[4];

int main(int, char *[])
{
    aids::String_Buffer sbuffer = {sizeof(buffer), buffer};
    aids::sprintln(&sbuffer, "Hello, World ", 69, ' ', 420, ' ', 3.1415f);
    aids::println(stdout, sbuffer.data);
    return 0;
}
