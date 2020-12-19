#include "../aids.hpp"

using namespace aids;

void prepare_expected_freq(Hash_Map<String_View, int> *map)
{
    map->insert("quis"_sv, 1);
    map->insert("voluptate"_sv, 1);
    map->insert("irure"_sv, 1);
    map->insert("exercitation"_sv, 1);
    map->insert("sit"_sv, 1);
    map->insert("qui"_sv, 1);
    map->insert("amet,"_sv, 1);
    map->insert("nisi"_sv, 1);
    map->insert("Excepteur"_sv, 1);
    map->insert("tempor"_sv, 1);
    map->insert("ad"_sv, 1);
    map->insert("anim"_sv, 1);
    map->insert("laboris"_sv, 1);
    map->insert("id"_sv, 1);
    map->insert("ipsum"_sv, 1);
    map->insert("esse"_sv, 1);
    map->insert("in"_sv, 3);
    map->insert("velit"_sv, 1);
    map->insert("aute"_sv, 1);
    map->insert("sint"_sv, 1);
    map->insert("consequat."_sv, 1);
    map->insert("sunt"_sv, 1);
    map->insert("incididunt"_sv, 1);
    map->insert("nostrud"_sv, 1);
    map->insert("officia"_sv, 1);
    map->insert("eiusmod"_sv, 1);
    map->insert("sed"_sv, 1);
    map->insert("fugiat"_sv, 1);
    map->insert("magna"_sv, 1);
    map->insert("aliqua."_sv, 1);
    map->insert("non"_sv, 1);
    map->insert("ullamco"_sv, 1);
    map->insert("commodo"_sv, 1);
    map->insert("mollit"_sv, 1);
    map->insert("do"_sv, 1);
    map->insert("culpa"_sv, 1);
    map->insert("adipiscing"_sv, 1);
    map->insert("nulla"_sv, 1);
    map->insert("ea"_sv, 1);
    map->insert("Ut"_sv, 1);
    map->insert("enim"_sv, 1);
    map->insert("reprehenderit"_sv, 1);
    map->insert("proident,"_sv, 1);
    map->insert("occaecat"_sv, 1);
    map->insert("pariatur."_sv, 1);
    map->insert("et"_sv, 1);
    map->insert("elit,"_sv, 1);
    map->insert("minim"_sv, 1);
    map->insert("eu"_sv, 1);
    map->insert("ex"_sv, 1);
    map->insert("dolor"_sv, 2);
    map->insert("laborum."_sv, 1);
    map->insert("dolore"_sv, 2);
    map->insert("cillum"_sv, 1);
    map->insert("ut"_sv, 2);
    map->insert("deserunt"_sv, 1);
    map->insert("est"_sv, 1);
    map->insert("aliquip"_sv, 1);
    map->insert("Lorem"_sv, 1);
    map->insert("cupidatat"_sv, 1);
    map->insert("veniam,"_sv, 1);
    map->insert("consectetur"_sv, 1);
    map->insert("labore"_sv, 1);
    map->insert("Duis"_sv, 1);
}

int main(int, char)
{
    String_View text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."_sv;

    Hash_Map<String_View, int> actual_freq = {};
    defer(destroy(actual_freq));
    Hash_Map<String_View, int> expected_freq = {};
    defer(destroy(expected_freq));

    prepare_expected_freq(&expected_freq);

    while (text.count > 0) {
        auto word = text.chop_word().trim();

        auto freq = actual_freq.get(word);
        if (freq.has_value) {
            *freq.unwrap += 1;
        } else {
            actual_freq.insert(word, 1);
        }
    }

    for (size_t i = 0; i < actual_freq.capacity; ++i) {
        if (actual_freq.buckets[i].has_value) {
            auto word = actual_freq.buckets[i].unwrap.key;
            auto freq = expected_freq.get(word);
            auto actual = actual_freq.buckets[i].unwrap.value;

            println(stdout, word, "...");

            if (!freq.has_value) {
                panic("ERROR: unexpected word `", word, "`");
            }

            auto expected = *freq.unwrap;

            if (expected != actual) {
                panic("ERROR: unexpected frequency of word `", word, "`. Expected: ", expected, ", Actual: ", actual);
            }
        }
    }

    if (actual_freq.size != expected_freq.size) {
        panic("ERROR: Unexpected size of hash map. ",
              "Expected: ", expected_freq.size, ", ",
              "Actual: ", actual_freq.size);
    }

    return 0;
}
