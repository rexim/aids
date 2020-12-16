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
    // String_View::has_suffix
    {
        String_View foo = "123456789"_sv;
        assert(true == foo.has_suffix(""_sv));
        assert(true == foo.has_suffix("9"_sv));
        assert(true == foo.has_suffix("89"_sv));
        assert(true == foo.has_suffix("789"_sv));
        assert(true == foo.has_suffix("6789"_sv));
        assert(true == foo.has_suffix("56789"_sv));
        assert(true == foo.has_suffix("456789"_sv));
        assert(true == foo.has_suffix("3456789"_sv));
        assert(true == foo.has_suffix("23456789"_sv));
        assert(true == foo.has_suffix("123456789"_sv));
        assert(false == foo.has_suffix("0123456789"_sv));
    }
    return 0;
}
