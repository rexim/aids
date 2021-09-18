#define AIDS_IMPLEMENTATION
#include "../aids.hpp"

using namespace aids;

#define ASSERT_EQ(expected_expr, actual_expr)              \
    do {                                                   \
        const auto expected = (expected_expr);             \
        const auto actual = (actual_expr);                 \
        if (expected != actual) {                          \
            println(stderr, __FILE__, ":", __LINE__,       \
                    ": ASSERTION FAILED! ",                \
                    #expected_expr, " == ", #actual_expr); \
            println(stderr, "  Expected: ", expected);     \
            println(stderr, "  Actual:   ", actual);       \
            exit(1);                                       \
        }                                                  \
    } while(0)

int main(int, char *[])
{
    // String_View::has_prefix
    {
        String_View foo = "123456789"_sv;
        ASSERT_EQ(true, foo.has_prefix(""_sv));
        ASSERT_EQ(true, foo.has_prefix("1"_sv));
        ASSERT_EQ(true, foo.has_prefix("12"_sv));
        ASSERT_EQ(true, foo.has_prefix("123"_sv));
        ASSERT_EQ(true, foo.has_prefix("1234"_sv));
        ASSERT_EQ(true, foo.has_prefix("12345"_sv));
        ASSERT_EQ(true, foo.has_prefix("123456"_sv));
        ASSERT_EQ(true, foo.has_prefix("1234567"_sv));
        ASSERT_EQ(true, foo.has_prefix("12345678"_sv));
        ASSERT_EQ(true, foo.has_prefix("123456789"_sv));
        ASSERT_EQ(false, foo.has_prefix("1234567890"_sv));
    }
    // String_View::has_suffix
    {
        String_View foo = "123456789"_sv;
        ASSERT_EQ(true, foo.has_suffix(""_sv));
        ASSERT_EQ(true, foo.has_suffix("9"_sv));
        ASSERT_EQ(true, foo.has_suffix("89"_sv));
        ASSERT_EQ(true, foo.has_suffix("789"_sv));
        ASSERT_EQ(true, foo.has_suffix("6789"_sv));
        ASSERT_EQ(true, foo.has_suffix("56789"_sv));
        ASSERT_EQ(true, foo.has_suffix("456789"_sv));
        ASSERT_EQ(true, foo.has_suffix("3456789"_sv));
        ASSERT_EQ(true, foo.has_suffix("23456789"_sv));
        ASSERT_EQ(true, foo.has_suffix("123456789"_sv));
        ASSERT_EQ(false, foo.has_suffix("0123456789"_sv));
    }
    // String_View::chop_left
    {
        // Normal
        {
            String_View foo = "123456789"_sv;
            String_View bar = foo.chop_left(4);
            ASSERT_EQ("56789"_sv, foo);
            ASSERT_EQ("1234"_sv, bar);
        }
        // Overchop
        {
            String_View foo = "123456789"_sv;
            String_View bar = foo.chop_left(69);
            ASSERT_EQ(""_sv, foo);
            ASSERT_EQ("123456789"_sv, bar);
        }
        // Underchop
        {
            String_View foo = "123456789"_sv;
            String_View bar = foo.chop_left(0);
            ASSERT_EQ("123456789"_sv, foo);
            ASSERT_EQ(""_sv, bar);
        }
    }
    // String_View::chop_right
    {
        // Normal
        {
            String_View foo = "123456789"_sv;
            String_View bar = foo.chop_right(5);
            ASSERT_EQ("1234"_sv, foo);
            ASSERT_EQ("56789"_sv, bar);
        }
        // Overchop
        {
            String_View foo = "123456789"_sv;
            String_View bar = foo.chop_right(69);
            ASSERT_EQ(""_sv, foo);
            ASSERT_EQ("123456789"_sv, bar);
        }
        // Underchop
        {
            String_View foo = "123456789"_sv;
            String_View bar = foo.chop_right(0);
            ASSERT_EQ("123456789"_sv, foo);
            ASSERT_EQ(""_sv, bar);
        }
    }
    // String_View::as_integer<int>
    {
        auto year = "2021"_sv;
        int number = unwrap_or_panic(year.as_integer<int>());
        number += 1000;
        ASSERT_EQ(3021, number);
    }
    return 0;
}
