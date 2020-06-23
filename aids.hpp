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
    T min(T a, T b)
    {
        return a < b ? a : b;
    }

    template <typename T>
    T max(T a, T b)
    {
        return a > b ? a : b;
    }

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

    template <typename Error>
    struct Result<void, Error>
    {
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

    template <typename T, typename Error, typename... Message>
    T unwrap_or_panic(Result<T, Error> result, Message... message)
    {
        if (result.is_error) {
            println(stdout, "[PANIC] ", message..., ": ", result.error);
            abort();
        }

        return result.unwrap;
    }

    template <typename Error, typename... Message>
    void unwrap_or_panic(Result<void, Error> result, Message... message)
    {
        if (result.is_error) {
            println(stdout, "[PANIC] ", message..., ": ", result.error);
            abort();
        }
    }

    template <typename U, typename T, typename Error>
    Result<U, Error> refail(Result<T, Error> result)
    {
        return {
            .is_error = result.is_error,
            .error = result.error
        };
    }

    template <typename T, typename Error>
    Result<T, Error> fail(Error error)
    {
        return {
            .is_error = true,
            .error = error
        };
    }

    ////////////////////////////////////////////////////////////
    // ALLOCATOR
    ////////////////////////////////////////////////////////////
    // Ator  = Allocator (like Ctor or Dtor)
    // Mator = Malloc Ator
    ////////////////////////////////////////////////////////////

    constexpr unsigned long long operator ""_KiB (unsigned long long s)
    {
        return s * 1024;
    }

    constexpr unsigned long long operator ""_MiB (unsigned long long s)
    {
        return s * 1024_KiB;
    }

    constexpr unsigned long long operator ""_GiB (unsigned long long s)
    {
        return s * 1024_MiB;
    }

    struct Mator {};

    template <typename T>
    Result<T, Errno> alloc(Mator *, size_t size)
    {
        T result = (T) malloc(size);
        if (result == nullptr) return result_errno<T>();
        return { .unwrap = result };
    }

    void free(Mator *, void *data, size_t)
    {
        std::free(data);
    }

    Mator mator;

    template <size_t Capacity = 640_KiB>
    struct Region
    {
        size_t size = 0;
        uint8_t data[Capacity];
    };

    template <typename T, size_t Capacity>
    Result<T, Errno> alloc(Region<Capacity> *region, size_t size)
    {
        if (size + region->size > Capacity) {
            return fail<T, Errno>({ ENOMEM });
        }

        T result = (T) (region->data + region->size);
        region->size += size;

        return { .unwrap = result };
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
    Result<const char *, Errno> cstr_of_string(String s, Ator *ator = &mator)
    {
        auto result = alloc<const char *>(ator, s.size + 1);
        if (result.is_error) return result;

        char* cstr = (char*)result.unwrap;

        memcpy((void *)cstr, s.data, s.size);
        cstr[s.size] = '\0';
        return { .unwrap = cstr };
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
    Result<String, Errno> copy(String s, Ator *ator = &mator)
    {
        auto memory = alloc(ator, s.size);
        if (memory.is_error) {
            return refail<String>(memory);
        }

        String result = {
            .size = s.size,
            .data = (const char *) memory.unwrap
        };

        memcpy((void*) result.data, s.data, result.size);

        return {.unwrap = result};
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

    String take(String s, size_t n)
    {
        return {
            .size = min(n, s.size),
            .data = s.data
        };
    }

    String drop(String s, size_t n)
    {
        if (n > s.size) return { .size = 0 };

        return {
            .size = s.size - n,
            .data = s.data + n
        };
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

    void print1(FILE *stream, unsigned long long x)
    {
        std::fprintf(stream, "%lld", x);
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

        auto data = alloc<char*>(ator, size);
        if (data.is_error) return refail<String>(data);

        size_t read_size = fread(data.unwrap, 1, size, f);
        if (read_size < size && ferror(f)) {
            return result_errno<String>();
        }

        return {
            .is_error = false,
            .unwrap = {
                .size = read_size,
                .data = (const char *)data.unwrap,
            }
        };
    }

    ////////////////////////////////////////////////////////////
    // QUEUE
    ////////////////////////////////////////////////////////////

    template <typename T, size_t Capacity>
    struct Fixed_Queue
    {
        size_t begin = 0;
        size_t size = 0;
        T elements[Capacity];
    };

    template <typename T, size_t Capacity>
    void enqueue(Fixed_Queue<T, Capacity> *queue, T element)
    {
        assert(queue->size < Capacity);
        queue->elements[(queue->begin + queue->size) % Capacity] = element;
        queue->size++;
    }

    template <typename T, size_t Capacity>
    T dequeue(Fixed_Queue<T, Capacity> *queue)
    {
        assert(queue->size > 0);
        T result = queue->elements[queue->begin];
        queue->begin = (queue->begin + 1) % Capacity;
        queue->size--;
        return result;
    }

    ////////////////////////////////////////////////////////////
    // STACK
    ////////////////////////////////////////////////////////////

    template <typename T, size_t Capacity>
    struct Fixed_Stack
    {
        size_t size = 0;
        T elements[Capacity];
    };

    template <typename T, size_t Capacity>
    Result<void, Errno> push(Fixed_Stack<T, Capacity> *stack, T element)
    {
        if (stack->size >= Capacity) {
            return fail<void, Errno>({ ENOMEM });
        }

        stack->elements[stack->size++] = element;

        return {};
    }
}

#endif  // AIDS_HPP_
