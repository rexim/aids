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
// aids — 0.21.0 — std replacement for C++. Designed to aid developers
// to a better programming experience.
//
// https://github.com/rexim/aids
//
// ============================================================
//
// ChangeLog (https://semver.org/ is implied)
//
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
    };

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

            return {true, result};
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

            return {true, result};
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

    Maybe<String_View> read_file_as_string_view(const char *filename)
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

        auto data = malloc(size);
        if (!data) return {};

        size_t read_size = fread(data, 1, size, f);
        if (read_size != (size_t) size && ferror(f)) return {};

        return {true, {static_cast<size_t>(size), static_cast<const char*>(data)}};
    }

    ////////////////////////////////////////////////////////////
    // DYNAMIC ARRAY
    ////////////////////////////////////////////////////////////

    template <typename T>
    struct Dynamic_Array
    {
        size_t capacity;
        size_t size;
        T *data;

        void expand_capacity()
        {
            capacity = data ? 2 * capacity : 256;
            data = (T*)realloc((void*)data, capacity * sizeof(T));
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

    ////////////////////////////////////////////////////////////
    // STRETCHY BUFFER
    ////////////////////////////////////////////////////////////

    namespace deprecated {
        struct Stretchy_Buffer
        {
            size_t capacity;
            size_t size;
            char *data;

            void push(const char *that_data, size_t that_size)
            {
                if (size + that_size > capacity) {
                    capacity = 2 * capacity + that_size;
                    data = (char*)realloc((void*)data, capacity);
                }

                memcpy(data + size, that_data, that_size);
                size += that_size;
            }

            template <typename T>
            void push(T x)
            {
                push((char*) &x, sizeof(x));
            }
        };
    }

    using Stretchy_Buffer [[deprecated("Use Dynamic_Array instead. Stretchy_Buffer is limited to only `char`-s while Dynamic_Array<T> can work with any type T.")]] = deprecated::Stretchy_Buffer;

    [[deprecated("Use Dynamic_Array instead. Stretchy_Buffer is limited to only `char`-s while Dynamic_Array<T> can work with any type T.")]]
    void print1(FILE *stream, deprecated::Stretchy_Buffer buffer)
    {
        fwrite(buffer.data, 1, buffer.size, stream);
    }

    ////////////////////////////////////////////////////////////
    // ARGS
    ////////////////////////////////////////////////////////////

    struct Args
    {
        int argc;
        char **argv;

        [[deprecated("Use Args::shift() instead. It was decided to rename `pop` to `shift` since it creates confusion with the pop operation of stacks which removes the elements from the other end. And shift is common operation in Bash and Perl (probably others) for parsing command line arguments.")]]
        char *pop()
        {
            return shift();
        }

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

    template <typename T, typename... Args>
    T unwrap_or_panic(Maybe<T> maybe, Args... args)
    {
        if (!maybe.has_value) {
            println(stderr, args...);
            exit(1);
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
            return {true, static_cast<uint32_t>(*view.data)};
        }

        if (view.count >= 2 &&
            (view.data[0] & UTF8_2BYTES_MASK) == 0 &&
            (view.data[1] & UTF8_EXTRA_BYTE_MASK) == 0)
        {
            *size = 2;
            const auto byte1 = static_cast<uint32_t>((view.data[0] & (UTF8_2BYTES_MASK - 1)) << 6);
            const auto byte2 = static_cast<uint32_t>(view.data[1] & (UTF8_EXTRA_BYTE_MASK - 1));
            return {true, byte1 | byte2};
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
            return {true, byte1 | byte2 | byte3};
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
            return {true, byte1 | byte2 | byte3 | byte4};
        }

        return {};
    }
}

#endif  // AIDS_HPP_
