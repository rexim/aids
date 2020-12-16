#include "aids.hpp"

using namespace aids;

int main(int argc, char *argv[])
{
    Hash_Map<String_View, float> map{};
    map.insert("foo"_sv, 2.0f);
    *map["bar"_sv] = 1.0f;

    assert(2.0f == *map["foo"_sv]);
    assert(1.0f == *map["bar"_sv]);

    // There was no such key as `"baz"_sv` beforehand,
    // so operator[] created one with a default value: `0.0f`
    assert(0.0f == *map["baz"_sv]);

    *map["baz"_sv] = 2020.f;
    assert(2020.f == *map["baz"_sv]);

    return 0;
}
