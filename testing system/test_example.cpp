/*
    Example for using the testing system
    #include "test.hpp"

    ADD_TEST(function, expected return value, function arguments)

    runTests(should program exit if a test fails)
*/

#include "test.hpp"
#include "../network simulator/sim.hpp"
#include <iostream>

struct ExampleStruct {
    int variable;
    char differentVariable;

    bool operator==(const ExampleStruct& other) const {
        return this->variable == other.variable && this->differentVariable == other.differentVariable;
    }
};

std::ostream& operator<<(std::ostream& os, const ExampleStruct& es) {
    os << "es.variable: " << es.variable;
    return os;
}

ExampleStruct foo(int a) {
    return ExampleStruct{.variable = a, .differentVariable = (char)a};
}

int main() {

    ExampleStruct es = {.variable = 10, .differentVariable = (char)10};

    for (int i = 0; i < 100; i++)
    {
        //These should pass
        ADD_TEST(foo, es, 10);

        //These should fail
        ADD_TEST(foo, es, 11);
    }
    
    bool allPassed = runTests(false);

    return !allPassed;
}
