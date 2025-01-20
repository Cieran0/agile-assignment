#pragma once

#include <iostream>
#include <functional>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>

// adds a test to the testQueue
#define ADD_TEST(function, expected_return_value, ...) _enqueueTest(#function, function, expected_return_value, __VA_ARGS__)

struct TestCase {
    std::string functionName;
    std::string failMesg;
    std::function<bool()> run;
};

extern std::vector<TestCase> testQueue;

template <typename Func, typename Expected, typename... Args>
void _enqueueTest(const char* functionName, Func func, Expected expected, Args... args) {

    std::stringstream failMesg;

    failMesg << "FAILED " << functionName << "(";
    ((failMesg << args << (sizeof...(args) > 1 ? ", " : "")), ...);
    failMesg << ") -> Expected: " << expected;
    testQueue.push_back({
        functionName,
        failMesg.str(),
        [=]() { 
            auto result = func(args...);
            bool passed = (result == expected);

            // Handle floating-point comparisons
            if constexpr (std::is_floating_point_v<Expected>) {
                passed = std::fabs(result - expected) < 1e-6; // Tolerance
            }
            return passed;
        }
    });
}

// Runs tests in the test_queue, if exit_on_fail, exits the program when a test fails
bool runTests(bool exitOnFail);

