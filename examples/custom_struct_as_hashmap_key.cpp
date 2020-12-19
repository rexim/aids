#include "aids.hpp"
using aids::operator""_sv;

struct Foo {
    int i{};
    aids::String_View sv{};
};

bool operator==(Foo lhs, Foo rhs) {
    return ((lhs.i == rhs.i) && (lhs.sv == rhs.sv));
}

// TODO: operator!=(Key, Key) needs to be implemented in Hash_Map<Key, Value>
// This time compiler message is quite meaningful (comparing to bug #8)
// Nevertheless, let's consider checking not only for `hash(Key)` (see #8),
// but `operator!=(Key, Key)` as well
bool operator!=(Foo lhs, Foo rhs) {
    return !(lhs == rhs);
}

unsigned long hash(int i)
{
    return i;
}

unsigned long hash(Foo foo)
{
    //TODO: hash() of custom type needs also bit shifting
    // This hashing is working, but an official example should comply
    // with the hashing principles
    return hash(foo.i) ^ hash(foo.sv);
}

int main(int, char *[])
{
    aids::Hash_Map<Foo, int> map{};
    *map[Foo{42, "forty-tooth"_sv}] = 42;

    auto foo1 = Foo{100000, "forty-tooth"_sv};
    auto foo2 = Foo{42, ""_sv};
    auto foo3 = Foo{42, "forty-tooth"_sv};

    assert(false == map.get(foo1).has_value);
    assert(false == map.get(foo2).has_value);
    assert(true == map.get(foo3).has_value);
    return 0;
}
