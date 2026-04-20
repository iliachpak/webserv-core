#ifndef TEST_RUNNER_HH
#define TEST_RUNNER_HH

#include <string>
#include <vector>

namespace test
{
    typedef bool (*TestFunc)();

    enum TestType 
    {
        TEST_CASE,
        TEST_SECTION_HEADER
    };

    struct Test
    {
        TestType type;
        std::string name;
        TestFunc    func;
    };

    class TestRunner
    {
    public:
        TestRunner();

        void addTest(const std::string& name, TestFunc func);
        void addSection(const std::string& name);
        int  run() const;

    private:
        std::vector<Test> _tests;
    };
}

#endif
