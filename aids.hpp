#ifndef AIDS_HPP_
#define AIDS_HPP_

#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace aids
{
    ////////////////////////////////////////////////////////////
    // RESULT
    ////////////////////////////////////////////////////////////

    template <typename T, typename Error>
    struct Result
    {
        T unwrap;
        bool is_error;
        Error error;
    };

    struct Errno
    {
        int unwrap;
    };

    void print1(FILE *stream, Errno e)
    {
        fprintf(stream, "%s", std::strerror(e.unwrap));
    }

    template <typename T>
    Result<T, Errno> result_errno(void)
    {
        return {
            .is_error = true,
            .error = { errno }
        };
    }

    ////////////////////////////////////////////////////////////
    // STRING
    ////////////////////////////////////////////////////////////

    struct String
    {
        size_t size;
        const char *data;

        // NOTE: this was not made a dtor because it is designed to be
        // used along with defer() to give you more flexible control
        // over the string's lifetime.
        void free()
        {
            std::free((void*) data);
        }

    };

    bool operator==(String a, String b)
    {
        if (a.size != b.size) return false;
        return memcmp(a.data, b.data, a.size) == 0;
    }

    bool operator!=(String a, String b)
    {
        return !(a == b);
    }

    String operator "" _s(const char *data, size_t size)
    {
        return { size, data };
    }

    String chop_by_delim(String *s, char delim)
    {
        if (s == nullptr || s->size == 0) {
            return {0};
        }

        size_t i = 0;
        while (i < s->size && s->data[i] != delim)
            ++i;

        String result = {
            .size = i,
            .data = s->data
        };

        if (i == s->size) {
            s->data += s->size;
            s->size = 0;
        } else {
            s->data += i + 1;
            s->size -= i + 1;
        }

        return result;
    }

    String trim_begin(String s)
    {
        while (s.size > 0 && isspace(*s.data)) {
            s.size -= 1;
            s.data += 1;
        }
        return s;
    }

    String trim_end(String s)
    {
        while (s.size > 0 && isspace(*(s.data + s.size - 1))) {
            s.size -= 1;
        }
        return s;
    }

    String trim(String s)
    {
        return trim_begin(trim_end(s));
    }

    ////////////////////////////////////////////////////////////
    // PRINT
    ////////////////////////////////////////////////////////////

    void print1(FILE *stream, String s)
    {
        std::fwrite(s.data, 1, s.size, stream);
    }

    void print1(FILE *stream, const char *s)
    {
        std::fwrite(s, 1, std::strlen(s), stream);
    }

    void print1(FILE *stream, char *s)
    {
        std::fwrite(s, 1, std::strlen(s), stream);
    }

    void print1(FILE *stream, char c)
    {
        std::fputc(c, stream);
    }

    void print1(FILE *stream, float f)
    {
        std::fprintf(stream, "%f", f);
    }

    template <typename ... Types>
    void print(FILE *stream, Types... args)
    {
        (print1(stream, args), ...);
    }

    template <typename ... Types>
    void println(FILE *stream, Types... args)
    {
        (print1(stream, args), ...);
        print1(stream, '\n');
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
#define defer(code)   auto DEFER_3(_defer_) = aids::defer_func([&](){code;})

    ////////////////////////////////////////////////////////////
    // FILE
    ////////////////////////////////////////////////////////////

    Result<String, Errno> read_whole_file(const char *filename)
    {
        FILE *f = fopen(filename, "rb");
        if (!f) return result_errno<String>();
        defer(fclose(f));

        int err = fseek(f, 0, SEEK_END);
        if (err < 0) return result_errno<String>();

        long size = ftell(f);
        if (size < 0) return result_errno<String>();

        err = fseek(f, 0, SEEK_SET);
        if (err < 0) return result_errno<String>();

        void *data = malloc(size);
        if (data == nullptr) return result_errno<String>();

        size_t read_size = fread(data, 1, size, f);
        if (read_size < size && ferror(f)) {
            return result_errno<String>();
        }

        return {
            .is_error = false,
            .unwrap = {
                .size = read_size,
                .data = (const char *)data,
            }
        };
    }
}

#endif  // AIDS_HPP_
