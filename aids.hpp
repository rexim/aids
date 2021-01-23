// Copyright 2020 Alexey Kutepov <reximkut@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// ============================================================
//
// aids — 1.1.0 — std replacement for C++. Designed to aid developers
// to a better programming experience.
//
// https://github.com/rexim/aids
//
// ============================================================
//
// ChangeLog (https://semver.org/ is implied)
//
//   1.1.0  add constexpr Maybe<T> some(T x)
//   1.0.0  remove Stretchy_Buffer{}
//          remove Args::pop()
//   0.40.0 Fix MSVC warnings
//   0.39.0 Fix common GCC warnings
//   0.38.0 struct Mtor{}
//          struct Fixed_Region{}
//          Make allocator for read_file_as_string_view customizable
//   0.37.0 NEVER HAPPENED
//   0.36.0 void destroy(String_View sv)
//   0.35.1 Fix compilation when using todo() and unreachable()
//   0.35.0 [[noreturn]] void unreachable(Args... args)
//          [[noreturn]] void todo(Args... args)
//   0.34.1 Fix -Wtype-limits warning in utf8_get_code()
//   0.34.0 Hash_Map::contains(Key key)
//   0.33.0 Maybe::value_or(T t)
//   0.32.0 Hash_Map::operator[](Key key)
//   0.31.0 String_View::has_suffix(String_View suffix)
//   0.30.0 String_View String_View::chop_while(Predicate_Char predicate)
//   0.29.0 void destroy(Dynamic_Array<T> dynamic_array)
//   0.28.0 struct Hash_Map
//   0.27.0 NEVER HAPPENED
//   0.26.0 panic() is marked with [[noreturn]] attribute
//          code_to_utf8() implementation is refactored in a backward compatible way
//   0.25.0 void print1(FILE *stream, Hex<char> hex)
//          void print1(FILE *stream, HEX<char> hex)
//          struct Hex_Bytes
//          void print1(FILE *stream, Hex_Bytes hex_bytes)
//   0.24.0 String_View Utf8_Char::view()
//          struct Hex
//          void print1(FILE *stream, Hex<uint32_t> hex)
//          struct HEX
//          void print1(FILE *stream, HEX<uint32_t> hex)
//   0.23.0 code_to_utf8()
//          struct Utf8_Char
//   0.22.0 panic()
//   0.21.0 void sprint1(String_Buffer *buffer, unsigned int x)
//   0.20.0 Escape
//   0.19.0 unwrap_or_panic()
//   0.18.0 Rename Args::pop() -> Args::shift()
//          Add more details to Stretchy_Buffer deprecation message
//   0.17.0 Dynamic_Array::concat()
//          Dynamic_Array::expand_capacity()
//   0.16.0 Dynamic_Array
//          deprecate Stretchy_Buffer
//   0.15.0 Make min() and max() variadic
//   0.14.0 size_t String_View::count_chars(char x) const
//   0.13.3 Fix control flow in utf8_get_code
//   0.13.2 Fix magic constant types in utf8_get_code
//   0.13.1 Remove macros from utf8_get_code implementation
//   0.13.0 void print1(FILE *stream, unsigned int x)
//          Maybe<uint32_t> utf8_get_code(String_View view, size_t *size)
//   0.12.1 Fix print1 and sprint1 bug for unsigned long long
//   0.12.0 void print1(FILE *stream, String_Buffer buffer)
//          void sprint1(String_Buffer *buffer, String_Buffer another_buffer)
//          String_View String_Buffer::view() const
//   0.11.0 Caps
//   0.10.0 sprint1(String_Buffer *buffer, String_View view)
//   0.9.0  String_Buffer
//          sprintln
//   0.8.0  Args
//   0.7.0  String_View::operator<()
//          print1(FILE*, bool)
//   0.6.0  swap
//   0.5.0  Equality operations for Maybe<T>
//   0.4.0  mod
//   0.3.0  Stretchy_Buffer
//   0.2.0  unwrap_into
//          print1 for long int
//   0.1.0  print1 for long unsigned int
//          print1 for int
//          Pad
//   0.0.3  bugfix for print1 of Maybe<T>
//   0.0.2  fix sign-unsigned integer comparison in aids::read_file_as_string_view
//   0.0.1  min, max, clamp,
//          defer,
//          Maybe<T>,
//          String_View,
//          print, println
//
// ============================================================
//
// Contributors:
//   Alexey Kutepov (github:rexim)
//   Aodhnait Étaín (github:aodhneine)
//   Jarosław Wiosna (github:JaroslawWiosna)
//   Danil Kolumbet (github:kolumb)

#ifndef AIDS_HPP_
#define AIDS_HPP_

#include <cassert>
#include <cctype>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace aids
{
    ////////////////////////////////////////////////////////////
    // ALLOCATORS
    ////////////////////////////////////////////////////////////

    struct Mtor
    {
        template <typename T>
        T *alloc(size_t count, T def = {})
        {
            T *result = static_cast<T*>(malloc(sizeof(T) * count));
            for (size_t i = 0; i < count; ++i) {
                result[i] = def;
            }
            return result;
        }

        template <typename T>
        void dealloc(T *ptr, size_t)
        {
            free(ptr);
        }

        template <typename T>
        void dealloc(const T *ptr, size_t)
        {
            free(const_cast<T*>(ptr));
        }
    };

    Mtor mtor;

    template <size_t Capacity>
    struct Fixed_Region
    {
        size_t size;
        char buffer[Capacity];

        template <typename T>
        T *alloc(size_t count, T def = T())
        {
            if (count * sizeof(T) > Capacity) {
                return nullptr;
            }

            T *result = static_cast<T*>(buffer + size);
            for (size_t i = 0; i < count; ++i) {
                result[i] = def;
            }

            size += count * sizeof(T);
            return result;
        }

        template <typename T>
        void dealloc(T*, size_t)
        {
        }

        template <typename T>
        void dealloc(const T*, size_t)
        {
        }

        void clean()
        {
            size = 0;
        }
    };

    ////////////////////////////////////////////////////////////
    // ALGORITHM
    ////////////////////////////////////////////////////////////

    template <typename T>
    T min(T x)
    {
        return x;
    }

    template <typename T, typename... Rest>
    T min(T x, Rest... rest)
    {
        auto y = min(rest...);
        return x < y ? x : y;
    }

    template <typename T>
    T max(T x)
    {
        return x;
    }

    template <typename T, typename... Rest>
    T max(T x, Rest... rest)
    {
        auto y = max(rest...);
        return x < y ? y : x;
    }

    template <typename T>
    T clamp(T x, T low, T high)
    {
        return min(max(low, x), high);
    }

    template <typename T>
    T mod(T a, T b)
    {
        return (a % b + b) % b;
    }

    template <typename T>
    void swap(T *a, T *b)
    {
        T t = *a;
        *a = *b;
        *b = t;
    }

    ////////////////////////////////////////////////////////////
    // DEFER
    ////////////////////////////////////////////////////////////

    // https://www.reddit.com/r/ProgrammerTIL/comments/58c6dx/til_how_to_defer_in_c/
    template <typename F>
    struct saucy_defer {
        F f;
        saucy_defer(F f) : f(f) {}
        ~saucy_defer() { f(); }
    };

    template <typename F>
    saucy_defer<F> defer_func(F f)
    {
        return saucy_defer<F>(f);
    }

#define DEFER_1(x, y) x##y
#define DEFER_2(x, y) DEFER_1(x, y)
#define DEFER_3(x)    DEFER_2(x, __COUNTER__)
#define defer(code)   auto DEFER_3(_defer_) = ::aids::defer_func([&](){code;})

    ////////////////////////////////////////////////////////////
    // MAYBE
    ////////////////////////////////////////////////////////////

    template <typename T>
    struct Maybe
    {
        bool has_value;
        T unwrap;

        bool operator!=(const Maybe<T> &that) const
        {
            return !(*this == that);
        }

        bool operator==(const Maybe<T> &that) const
        {
            if (this->has_value && that.has_value) {
                return this->unwrap == that.unwrap;
            }

            return !this->has_value && !that.has_value;
        }

        T value_or(T t) const {
            return (has_value ? unwrap : t);
        }
    };

    template <typename T>
    constexpr Maybe<T> some(T x)
    {
        return {true, x};
    }

#define unwrap_into(lvalue, maybe)              \
    do {                                        \
        auto maybe_var = (maybe);               \
        if (!maybe_var.has_value) return {};    \
        (lvalue) = maybe_var.unwrap;            \
    } while (0)

    ////////////////////////////////////////////////////////////
    // STRING_VIEW
    ////////////////////////////////////////////////////////////

    struct String_View
    {
        size_t count;
        const char *data;

        [[nodiscard]]
        String_View trim_begin(void) const
        {
            String_View view = *this;

            while (view.count != 0 && isspace(*view.data)) {
                view.data  += 1;
                view.count -= 1;
            }
            return view;
        }

        [[nodiscard]]
        String_View trim_end(void) const
        {
            String_View view = *this;

            while (view.count != 0 && isspace(*(view.data + view.count - 1))) {
                view.count -= 1;
            }
            return view;
        }

        [[nodiscard]]
        String_View trim(void) const
        {
            return trim_begin().trim_end();
        }

        void chop_back(size_t n)
        {
            count -= n < count ? n : count;
        }

        void chop(size_t n)
        {
            if (n > count) {
                data += count;
                count = 0;
            } else {
                data  += n;
                count -= n;
            }
        }

        void grow(size_t n)
        {
            count += n;
        }

        using Predicate_Char = bool (*)(char);

        String_View chop_while(Predicate_Char predicate)
        {
            size_t size = 0;
            while (size < count && predicate(data[size])) {
                size += 1;
            }

            auto result = subview(0, size);
            chop(size);
            return result;
        }

        String_View chop_by_delim(char delim)
        {
            assert(data);

            size_t i = 0;
            while (i < count && data[i] != delim) i++;
            String_View result = {i, data};
            chop(i + 1);

            return result;
        }

        String_View chop_word(void)
        {
            *this = trim_begin();

            size_t i = 0;
            while (i < count && !isspace(data[i])) i++;

            String_View result = { i, data };

            count -= i;
            data  += i;

            return result;
        }

        template <typename Integer>
        Maybe<Integer> from_hex() const
        {
            Integer result = {};

            for (size_t i = 0; i < count; ++i) {
                Integer x = data[i];
                if ('0' <= x && x <= '9') {
                    x = (Integer) (x - '0');
                } else if ('a' <= x && x <= 'f') {
                    x = (Integer) (x - 'a' + 10);
                } else if ('A' <= x && x <= 'F') {
                    x = (Integer) (x - 'A' + 10);
                } else {
                    return {};
                }
                result = result * (Integer) 0x10 + x;
            }

            return some(result);
        }

        template <typename Integer>
        Maybe<Integer> as_integer() const
        {
            Integer sign = 1;
            Integer number = {};
            String_View view = *this;

            if (view.count == 0) {
                return {};
            }

            if (*view.data == '-') {
                sign = -1;
                view.chop(1);
            }

            while (view.count) {
                if (!isdigit(*view.data)) {
                    return {};
                }
                number = number * 10 + (*view.data - '0');
                view.chop(1);
            }

            return { true, number * sign };
        }

        Maybe<float> as_float() const
        {
            char buffer[300] = {};
            memcpy(buffer, data, min(sizeof(buffer) - 1, count));
            char *endptr = NULL;
            float result = strtof(buffer, &endptr);

            if (buffer > endptr || (size_t) (endptr - buffer) != count) {
                return {};
            }

            return some(result);
        }


        String_View subview(size_t start, size_t count) const
        {
            if (start + count <= this->count) {
                return {count, data + start};
            }

            return {};
        }

        bool operator<(String_View b) const
        {
            auto a = *this;
            while (a.count > 0 && b.count > 0) {
                if (*a.data != *b.data) {
                    return *a.data < *b.data;
                }
                a.chop(1);
                b.chop(1);
            }

            return a.count < b.count;
        }

        bool operator==(String_View view) const
        {
            if (this->count != view.count) return false;
            return memcmp(this->data, view.data, this->count) == 0;
        }

        bool operator!=(String_View view) const
        {
            return !(*this == view);
        }

        bool has_prefix(String_View prefix) const
        {
            return prefix.count <= this->count
                && this->subview(0, prefix.count) == prefix;
        }

        bool has_suffix(String_View suffix) const
        {
            return suffix.count <= this->count
                && this->subview(this->count - suffix.count, suffix.count) == suffix;
        }

        size_t count_chars(char x) const
        {
            size_t result = 0;
            for (size_t i = 0; i < count; ++i) {
                if (data[i] == x) {
                    result += 1;
                }
            }
            return result;
        }
    };

    String_View operator ""_sv(const char *data, size_t count)
    {
        return {count, data};
    }

    String_View cstr_as_string_view(const char *cstr)
    {
        return {strlen(cstr), cstr};
    }

    void print1(FILE *stream, String_View view)
    {
        fwrite(view.data, 1, view.count, stream);
    }

    template <typename Ator = Mtor>
    Maybe<String_View> read_file_as_string_view(const char *filename,
                                                Ator *ator = &mtor)
    {
        FILE *f = fopen(filename, "rb");
        if (!f) return {};
        defer(fclose(f));

        int err = fseek(f, 0, SEEK_END);
        if (err < 0) return {};

        long size = ftell(f);
        if (size < 0) return {};

        err = fseek(f, 0, SEEK_SET);
        if (err < 0) return {};

        auto data = ator->template alloc<char>(size);
        if (!data) return {};

        size_t read_size = fread(data, 1, size, f);
        if (read_size != (size_t) size && ferror(f)) return {};

        return some(String_View {static_cast<size_t>(size), static_cast<const char*>(data)});
    }

    void destroy(String_View sv)
    {
        mtor.dealloc(sv.data, sv.count);
    }

    ////////////////////////////////////////////////////////////
    // DYNAMIC ARRAY
    ////////////////////////////////////////////////////////////

    template <typename T, typename Ator = Mtor>
    struct Dynamic_Array
    {
        size_t capacity;
        size_t size;
        T *data;

        void expand_capacity()
        {
            size_t new_capacity = data ? 2 * capacity : 256;
            T *new_data = mtor.alloc<T>(new_capacity);

            memcpy(new_data, data, capacity);

            data = new_data;
            capacity = new_capacity;
        }

        void push(T item)
        {
            while (size + 1 > capacity) {
                expand_capacity();
            }

            memcpy(data + size, &item, sizeof(T));
            size += 1;
        }

        void concat(const T *items, size_t items_count)
        {
            while (size + 1 > capacity) {
                expand_capacity();
            }

            memcpy(data + size, items, sizeof(T) * items_count);
            size += items_count;
        }

        bool contains(T item)
        {
            for (size_t i = 0; i < size; ++i) {
                if (item == data[i]) {
                    return true;
                }
            }

            return false;
        }
    };

    template <typename T>
    void destroy(Dynamic_Array<T> dynamic_array)
    {
        if (dynamic_array.data) {
            mtor.dealloc(dynamic_array.data, dynamic_array.capacity);
        }
    }

    ////////////////////////////////////////////////////////////
    // ARGS
    ////////////////////////////////////////////////////////////

    struct Args
    {
        int argc;
        char **argv;

        char *shift()
        {
            char *result = *argv;
            argv += 1;
            argc -= 1;
            return result;
        }

        bool empty()
        {
            return argc == 0;
        }
    };

    ////////////////////////////////////////////////////////////
    // SPRINT
    ////////////////////////////////////////////////////////////

    struct String_Buffer
    {
        size_t capacity;
        char *data;
        size_t size;

        String_View view() const
        {
            return {size, data};
        }
    };

    void sprint1(String_Buffer *buffer, const char *cstr)
    {
        int n = snprintf(
            buffer->data + buffer->size,
            buffer->capacity - buffer->size,
            "%s", cstr);
        buffer->size = min(buffer->size + n, buffer->capacity - 1);
    }

    void sprint1(String_Buffer *buffer, String_View view)
    {
        int n = snprintf(
            buffer->data + buffer->size,
            buffer->capacity - buffer->size,
            "%.*s", (int) view.count, view.data);
        buffer->size = min(buffer->size + n, buffer->capacity - 1);
    }

    void sprint1(String_Buffer *buffer, char c)
    {
        int n = snprintf(
            buffer->data + buffer->size,
            buffer->capacity - buffer->size,
            "%c", c);
        buffer->size = min(buffer->size + n, buffer->capacity - 1);
    }

    void sprint1(String_Buffer *buffer, float f)
    {
        int n = snprintf(
            buffer->data + buffer->size,
            buffer->capacity - buffer->size,
            "%f", f);
        buffer->size = min(buffer->size + n, buffer->capacity - 1);
    }

    void sprint1(String_Buffer *buffer, unsigned long long x)
    {
        int n = snprintf(
            buffer->data + buffer->size,
            buffer->capacity - buffer->size,
            "%llu", x);
        buffer->size = min(buffer->size + n, buffer->capacity - 1);
    }

    void sprint1(String_Buffer *buffer, unsigned int x)
    {
        int n = snprintf(
            buffer->data + buffer->size,
            buffer->capacity - buffer->size,
            "%u", x);
        buffer->size = min(buffer->size + n, buffer->capacity - 1);
    }

    void sprint1(String_Buffer *buffer, long unsigned int x)
    {
        int n = snprintf(
            buffer->data + buffer->size,
            buffer->capacity - buffer->size,
            "%lu", x);
        buffer->size = min(buffer->size + n, buffer->capacity - 1);
    }

    void sprint1(String_Buffer *buffer, int x)
    {
        int n = snprintf(
            buffer->data + buffer->size,
            buffer->capacity - buffer->size,
            "%d", x);
        buffer->size = min(buffer->size + n, buffer->capacity - 1);
    }

    void sprint1(String_Buffer *buffer, long int x)
    {
        int n = snprintf(
            buffer->data + buffer->size,
            buffer->capacity - buffer->size,
            "%ld", x);
        buffer->size = min(buffer->size + n, buffer->capacity - 1);
    }

    void sprint1(String_Buffer *buffer, bool b)
    {
        sprint1(buffer, b ? "true" : "false");
    }

    template <typename ... Types>
    void sprint(String_Buffer *buffer, Types... args)
    {
        (sprint1(buffer, args), ...);
    }

    template <typename T>
    void sprint1(String_Buffer *buffer, Maybe<T> maybe)
    {
        if (!maybe.has_value) {
            sprint(buffer, "None");
        } else {
            sprint(buffer, "Some(", maybe.unwrap, ")");
        }
    }

    template <typename ... Types>
    void sprintln(String_Buffer *buffer, Types... args)
    {
        (sprint1(buffer, args), ...);
        sprint1(buffer, '\n');
    }

    struct Pad
    {
        size_t n;
        char c;
    };

    void sprint1(String_Buffer *buffer, Pad pad)
    {
        for (size_t i = 0; i < pad.n; ++i) {
            sprint1(buffer, pad.c);
        }
    }

    struct Caps
    {
        String_View unwrap;
    };

    void sprint1(String_Buffer *buffer, Caps caps)
    {
        for (size_t i = 0; i < caps.unwrap.count; ++i) {
            sprint1(buffer, (char) toupper(caps.unwrap.data[i]));
        }
    }

    void sprint1(String_Buffer *buffer, String_Buffer another_buffer)
    {
        sprint1(buffer, another_buffer.view());
    }

    struct Escape
    {
        String_View unwrap;
    };

    ////////////////////////////////////////////////////////////
    // PRINT
    ////////////////////////////////////////////////////////////

    void print1(FILE *stream, const char *s)
    {
        fwrite(s, 1, strlen(s), stream);
    }

    void print1(FILE *stream, char *s)
    {
        fwrite(s, 1, strlen(s), stream);
    }

    void print1(FILE *stream, char c)
    {
        fputc(c, stream);
    }

    void print1(FILE *stream, float f)
    {
        fprintf(stream, "%f", f);
    }

    void print1(FILE *stream, unsigned long long x)
    {
        fprintf(stream, "%llu", x);
    }

    void print1(FILE *stream, long unsigned int x)
    {
        fprintf(stream, "%lu", x);
    }

    void print1(FILE *stream, unsigned int x)
    {
        fprintf(stream, "%u", x);
    }

    void print1(FILE *stream, int x)
    {
        fprintf(stream, "%d", x);
    }

    void print1(FILE *stream, long int x)
    {
        fprintf(stream, "%ld", x);
    }

    template <typename ... Types>
    void print(FILE *stream, Types... args)
    {
        (print1(stream, args), ...);
    }

    void print1(FILE *stream, bool b)
    {
        print1(stream, b ? "true" : "false");
    }

    template <typename T>
    void print1(FILE *stream, Maybe<T> maybe)
    {
        if (!maybe.has_value) {
            print(stream, "None");
        } else {
            print(stream, "Some(", maybe.unwrap, ")");
        }
    }

    template <typename ... Types>
    void println(FILE *stream, Types... args)
    {
        (print1(stream, args), ...);
        print1(stream, '\n');
    }

    template <typename... Args>
    [[noreturn]] void panic(Args... args)
    {
        println(stderr, args...);
        exit(1);
    }

    template <typename... Args>
    [[noreturn]] void unreachable(Args... args)
    {
        panic("Unreachable: ", args...);
    }

    template <typename... Args>
    [[noreturn]] void todo(Args... args)
    {
        panic("TODO: ", args...);
    }

    template <typename T, typename... Args>
    T unwrap_or_panic(Maybe<T> maybe, Args... args)
    {
        if (!maybe.has_value) {
            panic(args...);
        }

        return maybe.unwrap;
    }

    void print1(FILE *stream, Escape escape)
    {
        for (size_t i = 0; i < escape.unwrap.count; ++i) {
            switch (escape.unwrap.data[i]) {
            case '\a': print(stream, "\\a"); break;
            case '\b': print(stream, "\\b"); break;
            case '\f': print(stream, "\\f"); break;
            case '\n': print(stream, "\\n"); break;
            case '\r': print(stream, "\\r"); break;
            case '\t': print(stream, "\\t"); break;
            case '\v': print(stream, "\\v"); break;
            default: print(stream, escape.unwrap.data[i]);
            }
        }
    }

    void print1(FILE *stream, Pad pad)
    {
        for (size_t i = 0; i < pad.n; ++i) {
            fputc(pad.c, stream);
        }
    }

    void print1(FILE *stream, Caps caps)
    {
        for (size_t i = 0; i < caps.unwrap.count; ++i) {
            print1(stream, (char) toupper(caps.unwrap.data[i]));
        }
    }

    void print1(FILE *stream, String_Buffer buffer)
    {
        print1(stream, buffer.view());
    }

    ////////////////////////////////////////////////////////////
    // UTF-8
    ////////////////////////////////////////////////////////////

    struct Utf8_Char {
        uint8_t bytes[4];
        size_t count;

        String_View view()
        {
            String_View result = {
                count,
                reinterpret_cast<const char *>(bytes)
            };

            return result;
        }
    };

    void print1(FILE *stream, Utf8_Char uchar)
    {
        print(stream, String_View {uchar.count, reinterpret_cast<const char*>(uchar.bytes)});
    }

    Utf8_Char code_to_utf8(uint32_t code)
    {
        if (/*0x0000 <= code && */code <= 0x007F) {
            // 0xxxxxxx
            // 1 byte
            return Utf8_Char {
                {(uint8_t) code, 0, 0, 0},
                1,
            };
        } else if (0x0080 <= code && code <= 0x07FF) {
            // 110xxxxx 10xxxxxx
            // 2 bytes
            return Utf8_Char {
                {
                    (uint8_t) (((code & 0b00111111000000) >> 6) | 0b11000000),
                    (uint8_t) (((code & 0b00000000111111) >> 0) | 0b10000000),
                    0,
                    0
                },
                2
            };
        } else if (0x0800 <= code && code <= 0xFFFF) {
            // 3 bytes
            // 1110xxxx 10xxxxxx 10xxxxxx
            return Utf8_Char {
                {
                    (uint8_t) (((code & 0b1111000000000000) >> 12) | 0b11100000),
                    (uint8_t) (((code & 0b0000111111000000) >> 6)  | 0b10000000),
                    (uint8_t) (((code & 0b0000000000111111) >> 0)  | 0b10000000),
                    0
                },
                3
            };
        } else if (0x10000 <= code && code <= 0x10FFFF) {
            // 4 bytes
            // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            return Utf8_Char {
                {
                    (uint8_t) (((code & 0b111000000000000000000) >> 18) | 0b11110000),
                    (uint8_t) (((code & 0b000111111000000000000) >> 12) | 0b10000000),
                    (uint8_t) (((code & 0b000000000111111000000) >> 6)  | 0b10000000),
                    (uint8_t) (((code & 0b000000000000000111111) >> 0)  | 0b10000000),
                },
                4
            };
        }

        panic("The code ", code, " point is too big");
    }

    Maybe<uint32_t> utf8_get_code(String_View view, size_t *size)
    {
        const uint8_t UTF8_1BYTE_MASK      = 1 << 7;
        const uint8_t UTF8_2BYTES_MASK     = 1 << 5;
        const uint8_t UTF8_3BYTES_MASK     = 1 << 4;
        const uint8_t UTF8_4BYTES_MASK     = 1 << 3;
        const uint8_t UTF8_EXTRA_BYTE_MASK = 1 << 6;

        if (view.count >= 1 &&
            (*view.data & UTF8_1BYTE_MASK) == 0)
        {
            *size = 1;
            return some(static_cast<uint32_t>(*view.data));
        }

        if (view.count >= 2 &&
            (view.data[0] & UTF8_2BYTES_MASK) == 0 &&
            (view.data[1] & UTF8_EXTRA_BYTE_MASK) == 0)
        {
            *size = 2;
            const auto byte1 = static_cast<uint32_t>((view.data[0] & (UTF8_2BYTES_MASK - 1)) << 6);
            const auto byte2 = static_cast<uint32_t>(view.data[1] & (UTF8_EXTRA_BYTE_MASK - 1));
            return some(byte1 | byte2);
        }

        if (view.count >= 3 &&
            (view.data[0] & UTF8_3BYTES_MASK) == 0 &&
            (view.data[1] & UTF8_EXTRA_BYTE_MASK) == 0 &&
            (view.data[2] & UTF8_EXTRA_BYTE_MASK) == 0)
        {
            *size = 3;
            const auto byte1 = static_cast<uint32_t>((view.data[0] & (UTF8_3BYTES_MASK - 1)) << (6 * 2));
            const auto byte2 = static_cast<uint32_t>((view.data[1] & (UTF8_EXTRA_BYTE_MASK - 1)) << 6);
            const auto byte3 = static_cast<uint32_t>(view.data[2] & (UTF8_EXTRA_BYTE_MASK - 1));
            return some(byte1 | byte2 | byte3);
        }

        if (view.count >= 4 &&
            (view.data[0] & UTF8_4BYTES_MASK) == 0 &&
            (view.data[1] & UTF8_EXTRA_BYTE_MASK) == 0 &&
            (view.data[2] & UTF8_EXTRA_BYTE_MASK) == 0 &&
            (view.data[3] & UTF8_EXTRA_BYTE_MASK) == 0)
        {
            *size = 4;
            const auto byte1 = static_cast<uint32_t>((view.data[0] & (UTF8_3BYTES_MASK - 1)) << (6 * 3));
            const auto byte2 = static_cast<uint32_t>((view.data[1] & (UTF8_EXTRA_BYTE_MASK - 1)) << (6 * 2));
            const auto byte3 = static_cast<uint32_t>((view.data[2] & (UTF8_EXTRA_BYTE_MASK - 1)) << 6);
            const auto byte4 = static_cast<uint32_t>(view.data[3] & (UTF8_EXTRA_BYTE_MASK - 1));
            return some(byte1 | byte2 | byte3 | byte4);
        }

        return {};
    }

    template <typename T>
    struct Hex
    {
        T unwrap;
    };

    void print1(FILE *stream, Hex<uint32_t> hex)
    {
        fprintf(stream, "%x", hex.unwrap);
    }

    void print1(FILE *stream, Hex<char> hex)
    {
        fprintf(stream, "%hhx", hex.unwrap);
    }

    template <typename T>
    struct HEX
    {
        T unwrap;
    };

    void print1(FILE *stream, HEX<uint32_t> hex)
    {
        fprintf(stream, "%X", hex.unwrap);
    }

    void print1(FILE *stream, HEX<char> hex)
    {
        fprintf(stream, "%hhX", hex.unwrap);
    }

    struct Hex_Bytes
    {
        String_View unwrap;
    };

    void print1(FILE *stream, Hex_Bytes hex_bytes)
    {
        print(stream, "[");
        for (size_t i = 0; i < hex_bytes.unwrap.count; ++i) {
            print(stream, i == 0 ? "" : ", ", Hex<char> { hex_bytes.unwrap.data[i] });
        }
        print(stream, "]");
    }

    ////////////////////////////////////////////////////////////
    // Hash_Map
    ////////////////////////////////////////////////////////////

    // NOTE: stolen from http://www.cse.yorku.ca/~oz/hash.html
    unsigned long hash(String_View str)
    {
        unsigned long hash = 5381;
        for (size_t i = 0; i < str.count; ++i) {
            hash = ((hash << 5) + hash) + str.data[i];
        }
        return hash;
    }

    template <typename Key, typename Value>
    struct Hash_Map
    {
        struct Bucket
        {
            Key key;
            Value value;
        };

        Maybe<Bucket> *buckets;
        size_t capacity;
        size_t size;

        void extend_capacity()
        {
            const size_t HASH_MAP_INITIAL_CAPACITY = 256;

            if (buckets == nullptr) {
                assert(capacity == 0);
                assert(size == 0);

                buckets = mtor.alloc<Maybe<Bucket>>(HASH_MAP_INITIAL_CAPACITY);
                capacity = HASH_MAP_INITIAL_CAPACITY;
                size = 0;
            } else {
                Hash_Map<Key, Value> new_hash_map = {
                    mtor.alloc<Maybe<Bucket>>(capacity * 2),
                    capacity * 2,
                    0
                };

                for (size_t i = 0; i < capacity; ++i) {
                    if (buckets[i].has_value) {
                        new_hash_map.insert(
                            buckets[i].unwrap.key,
                            buckets[i].unwrap.value);
                    }
                }

                mtor.dealloc(buckets, capacity);

                *this = new_hash_map;
            }
        }

        void insert(Key key, Value value)
        {
            if (size >= capacity) {
                extend_capacity();
            }

            auto hk = hash(key) & (capacity - 1);
            while (buckets[hk].has_value && buckets[hk].unwrap.key != key) {
                hk = (hk + 1) & (capacity - 1);
            }
            buckets[hk].has_value = true;
            buckets[hk].unwrap.key = key;
            buckets[hk].unwrap.value = value;
            size += 1;
        }

        Maybe<Value*> get(Key key)
        {
            auto hk = hash(key) & (capacity - 1);
            for (size_t i = 0;
                 i < capacity
                     && buckets[hk].has_value
                     && buckets[hk].unwrap.key != key;
                 ++i) {
                hk = (hk + 1) & (capacity - 1);
            }

            if (buckets && buckets[hk].has_value && buckets[hk].unwrap.key == key) {
                return some(&buckets[hk].unwrap.value);
            } else {
                return {};
            }
        }

        bool contains(Key key) {
            return get(key).has_value;
        }

        Value *operator[](Key key)
        {
            {
                Maybe<Value*> maybe_value = get(key);
                if (!maybe_value.has_value) {
                    insert(key, {});
                } else {
                    return maybe_value.unwrap;
                }
            }
            Maybe<Value*> maybe_value = get(key);
            assert(maybe_value.has_value);
            return maybe_value.unwrap;
        }
    };

    template <typename Key, typename Value>
    void destroy(Hash_Map<Key, Value> hash_map)
    {
        if (hash_map.buckets) {
            mtor.dealloc(hash_map.buckets, hash_map.capacity);
        }
    }
}

#endif  // AIDS_HPP_
