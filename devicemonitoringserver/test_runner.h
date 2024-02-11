#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include <exception>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& s)
{
    os << "[";
    bool first = true;
    for (const auto& x : s)
    {
        if (!first)
        {
            os << ", ";
        }
        first = false;
        os << x;
    }
    return os << "]";
}

template <class T>
std::ostream& operator<<(std::ostream& os, const std::set<T>& s)
{
    os << "{";
    bool first = true;
    for (const auto& x : s)
    {
        if (!first)
        {
            os << ", ";
        }
        first = false;
        os << x;
    }
    return os << "}";
}

template <class K, class V>
std::ostream& operator<<(std::ostream& os, const std::map<K, V>& m)
{
    os << "{";
    bool first = true;
    for (const auto& kv : m)
    {
        if (!first)
        {
            os << ", ";
        }
        first = false;
        os << kv.first << ": " << kv.second;
    }
    return os << "}";
}

template <class K, class V>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<K, V>& m)
{
    os << "{";
    bool first = true;
    for (const auto& kv : m)
    {
        if (!first)
        {
            os << ", ";
        }
        first = false;
        os << kv.first << ": " << kv.second;
    }
    return os << "}";
}

template <class T, class U>
void AssertEqual(const T& t, const U& u,
                 const std::string& hint)
{
    if (t != u)
    {
        std::ostringstream os;
        os << "Assertion failed: " << t << " != " << u
           << " hint: " << hint;
        throw std::runtime_error(os.str());
    }
}

template <class T, class U>
void AssertNotEqual(const T& t, const U& u,
                    const std::string& hint)
{
    if (t == u)
    {
        std::ostringstream os;
        os << "Assertion failed: " << t << " == " << u
           << " hint: " << hint;
        throw std::runtime_error(os.str());
    }
}

template <class T, class U, class Y>
void AssertWithThreshold(T t, U u, Y threshold,
                         const std::string& hint)
{
    if (!(t + threshold >= u && t - threshold <= u))
    {
        std::ostringstream os;
        os << "Assertion failed: " << t << " != " << u << " +- " << threshold
           << " hint: " << hint;
        throw std::runtime_error(os.str());
    }
}

inline void Assert(bool b, const std::string& hint)
{
    AssertEqual(b, true, hint);
}

class TestRunner
{
public:
    template <class TestFunc>
    void RunTest(TestFunc func, const std::string& test_name)
    {
        try
        {
            func();
            std::cerr << test_name << " OK" << std::endl;
        }
        catch (std::exception& e)
        {
            ++fail_count;
            std::cerr << test_name << " fail: " << e.what() << std::endl;
        }
    }

    ~TestRunner()
    {
        if (fail_count > 0)
        {
            std::cerr << fail_count << " unit tests failed. Terminate" << std::endl;
            exit(1);
        }
        else
        {
            std::cout << "*** All tests OK ***" << std::endl;
        }
    }

private:
    int fail_count = 0;
};

#define UNIQUE_2(x) name##x
#define UNIQUE_1(x) UNIQUE_2(x)
#define UNIQUE      UNIQUE_1(__LINE__)

// После вставки все оказываются в одной строке -> Можно использовать UNIQUE
#define ASSERT_EQUAL(x, y)                                                   \
{                                                                        \
        std::ostringstream UNIQUE;                                           \
        UNIQUE << #x << " != " << #y << ", " << __FILE__ << ":" << __LINE__; \
        AssertEqual(x, y, UNIQUE.str());                                     \
}

#define ASSERT_NOT_EQUAL(x, y)                                               \
{                                                                        \
        std::ostringstream UNIQUE;                                           \
        UNIQUE << #x << " == " << #y << ", " << __FILE__ << ":" << __LINE__; \
        AssertNotEqual(x, y, UNIQUE.str());                                  \
}

#define ASSERT_WITH_THRESHOLD(x, y, threshold)                                                      \
{                                                                                               \
        std::ostringstream UNIQUE;                                                                  \
        UNIQUE << #x << " != " << #y << " +- " << #threshold << ", " << __FILE__ << ":" << __LINE__; \
        AssertWithThreshold(x, y, threshold, UNIQUE.str());                                         \
}

#define ASSERT(x)                                                     \
{                                                                 \
        std::ostringstream UNIQUE;                                    \
        UNIQUE << #x << " is false, " << __FILE__ << ":" << __LINE__; \
        Assert(x, UNIQUE.str());                                      \
}

#define RUN_TEST(tr, func) \
tr.RunTest(func, #func)

#endif //TEST_RUNNER_H
