#include "TestRunner.hh"
#include <iostream>

namespace test
{
    TestRunner::TestRunner() {}

    void TestRunner::addTest(const std::string& name, TestFunc func)
    {
        Test t;
        t.type = TEST_CASE;
        t.name = name;
        t.func = func;
        _tests.push_back(t);
    }

    void TestRunner::addSection(const std::string& name) 
    {
        Test t;
        t.type = TEST_SECTION_HEADER;
        t.name = name;
        t.func = NULL;
        _tests.push_back(t);
    }

    int TestRunner::run() const
    {
        int failures = 0;
        int headers = 0;

        for (std::vector<Test>::const_iterator it = _tests.begin();
             it != _tests.end();
             ++it)
        {
            if (it->type == TEST_SECTION_HEADER)
            {
                std::cout << "================= " << it->name << " =================";
                headers++;
            }
            else if (it->type == TEST_CASE) 
            {
                std::cout << "[TEST] " << it->name << " ... ";

                bool ok = false;
                try {
                    ok = it->func();
                }
                catch (...) {
                    ok = false;
                }

                if (ok)
                    std::cout << "OK";
                else
                {
                    std::cout << "FAILED";
                    ++failures;
                }
            }

            std::cout << std::endl;
        }

        std::cout << std::endl;
        int testsNbr = _tests.size() - headers;
        std::cout << "Summary: "
                  << (testsNbr - failures)
                  << "/" << testsNbr
                  << " tests passed" << std::endl;

        return failures == 0 ? 0 : 1;
    }
}
