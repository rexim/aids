#include "../aids.hpp"

using namespace aids;

int main(int, char)
{
    const uint32_t UNICODE_LOW = 0x0000;
    const uint32_t UNICODE_HIGH = 0x10FFFF;

    println(stdout, "Checking decoding/encoding of all code points within the range ",
            "[0x", HEX<uint32_t> { UNICODE_LOW }, "..0x", HEX<uint32_t> { UNICODE_HIGH }, "]...");

    for (uint32_t expected_code = UNICODE_LOW;
         expected_code <= UNICODE_HIGH;
         ++expected_code)
    {
        auto uchar = code_to_utf8(expected_code);
        auto uchar_view = uchar.view();

        size_t size = 0;
        auto actual_code = unwrap_or_panic(
            utf8_get_code(uchar_view, &size),
            "FAILED: could not decode UTF-8 sequence ", Hex_Bytes { uchar_view },
            " into a Unicode code point ", HEX<uint32_t> { expected_code });

        if (expected_code != actual_code) {
            panic("FAILED: ",
                  "expected code ", HEX<uint32_t> { expected_code }, ", ",
                  "but got ", HEX<uint32_t> { actual_code }, ".");

        }

        if (size != uchar.count) {
            panic("FAILED: ",
                  "expected size of code ", HEX<uint32_t> { expected_code }, " ",
                  "to be ", uchar.count, ", but it was ", size, ".");
        }
    }

    println(stdout, "OK.");

    return 0;
}
