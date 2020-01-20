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
    // ALLOCATOR
    ////////////////////////////////////////////////////////////

    constexpr unsigned long long operator ""_Kb (unsigned long long s)
    {
        return s * 1024;
    }

    constexpr unsigned long long operator ""_Mb (unsigned long long s)
    {
        return s * 1024_Kb;
    }

    constexpr unsigned long long operator ""_Gb (unsigned long long s)
    {
        return s * 1024_Mb;
    }

    struct Mator {};

    // FIXME: alloc result should be templated

    void *alloc(Mator *, size_t size)
    {
        return malloc(size);
    }

    void free(Mator *, void *data, size_t)
    {
        std::free(data);
    }

    Mator mator;

    template <size_t Capacity = 640_Kb>
    struct Region
    {
        size_t size = 0;
        uint8_t data[Capacity];
    };

    template <size_t Capacity>
    void *alloc(Region<Capacity> *region, size_t size)
    {
        assert(size + region->size <= Capacity);
        void *result = region->data + region->size;
        region->size += size;
        return result;
    }

    ////////////////////////////////////////////////////////////
    // STRING
    ////////////////////////////////////////////////////////////

    struct String
    {
        size_t size;
        const char *data;
    };

    String string_of_cstr(const char *data)
    {
        String result = {
            .size = strlen(data),
            .data = data
        };
        return result;
    }

    template <typename Ator = Mator>
    void free(String s, Ator *ator = &mator)
    {
        free(ator, (void*) s.data, s.size);
    }

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

    template <typename Ator = Mator>
    String copy(String s, Ator *ator = &mator)
    {
        String result = {
            .size = s.size,
            .data = (const char *) alloc(ator, s.size)
        };

        memcpy((void*) result.data, s.data, result.size);

        return result;
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

    template <typename Ator = Mator>
    Result<String, Errno> read_whole_file(const char *filename,
                                          Ator *ator = &mator)
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

        void *data = alloc(ator, size);
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
