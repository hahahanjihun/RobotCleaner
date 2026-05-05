#include "test_positive.h"
#include "test_negative.h"

#include <iostream>
#include <vector>
#include <string>

struct UnitTestCase {
    std::string id;
    bool (*func)();
};

int main() {
    std::vector<UnitTestCase> tests = {
        {"TC-POS-01", TC_POS_01},
        {"TC-POS-02", TC_POS_02},
        {"TC-POS-03", TC_POS_03},
        {"TC-POS-04", TC_POS_04},
        {"TC-POS-05", TC_POS_05},
        {"TC-POS-06", TC_POS_06},
        {"TC-POS-07", TC_POS_07},
        {"TC-POS-08", TC_POS_08},
        {"TC-POS-09", TC_POS_09},
        {"TC-POS-10", TC_POS_10},

        {"TC-NEG-01", TC_NEG_01},
        {"TC-NEG-02", TC_NEG_02},
        {"TC-NEG-03", TC_NEG_03},
        {"TC-NEG-04", TC_NEG_04},
        {"TC-NEG-05", TC_NEG_05},
        {"TC-NEG-06", TC_NEG_06},
        {"TC-NEG-07", TC_NEG_07},
        {"TC-NEG-08", TC_NEG_08},
        {"TC-NEG-09", TC_NEG_09},
        {"TC-NEG-10", TC_NEG_10},
        {"TC-NEG-11", TC_NEG_11},
        {"TC-NEG-12", TC_NEG_12},
        {"TC-NEG-13", TC_NEG_13},
        {"TC-NEG-14", TC_NEG_14},
        {"TC-NEG-15", TC_NEG_15},
        {"TC-NEG-16", TC_NEG_16},
        {"TC-NEG-17", TC_NEG_17},
        {"TC-NEG-18", TC_NEG_18},
        {"TC-NEG-19", TC_NEG_19},
        {"TC-NEG-20", TC_NEG_20}
    };

    int passed = 0;

    for (const auto& test : tests) {
        bool result = test.func();

        std::cout << test.id << " : "
                  << (result ? "PASS" : "FAIL")
                  << std::endl;

        if (result) {
            passed++;
        }
    }

    std::cout << "Unit Test Result: "
              << passed << " / " << tests.size()
              << " passed" << std::endl;

    return passed == static_cast<int>(tests.size()) ? 0 : 1;
}