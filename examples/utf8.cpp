#include "aids.hpp"

using namespace aids;

void code_points_of(const char *cstr)
{
    String_View s = cstr_as_string_view(cstr);
    println(stdout, "Code points of \"", s, "\"");
    while (s.count > 0) {
        size_t size = 0;
        auto code = utf8_get_code(s, &size);
        if (code.has_value) {
            println(stdout, s.subview(0, size), " => ", code.unwrap);
            s.chop(size);
        } else {
            assert(cstr <= s.data);
            println(stderr, "STRING CONTAINS INVALID UTF-8 BYTE SEQUENCE AT BYTE INDEX ", (size_t) (s.data - cstr), "!!!");
            abort();
        }
    }
}

int main(int, char)
{
    code_points_of("Hello, World!");
    code_points_of("Привет, Мир!");
    code_points_of("こんにちは世界!");
    code_points_of("😂👌💯🔥");

    return 0;
}
