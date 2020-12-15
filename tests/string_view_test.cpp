#include "../aids.hpp"

using namespace aids;

int main(int argc, char *argv[])
{
    // String_View::has_prefix
    {
        String_View foo = "123456789"_sv;
        assert(true == foo.has_prefix(""_sv));
        assert(true == foo.has_prefix("1"_sv));
        assert(true == foo.has_prefix("12"_sv));
        assert(true == foo.has_prefix("123"_sv));
        assert(true == foo.has_prefix("1234"_sv));
        assert(true == foo.has_prefix("12345"_sv));
        assert(true == foo.has_prefix("123456"_sv));
        assert(true == foo.has_prefix("1234567"_sv));
        assert(true == foo.has_prefix("12345678"_sv));
        assert(true == foo.has_prefix("123456789"_sv));
        assert(false == foo.has_prefix("1234567890"_sv));
    }
    return 0;
}
