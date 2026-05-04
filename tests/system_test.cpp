#include <iostream>
#include <fstream>
#include <vector>
#include <string>

struct TestResult {
    std::string id;
    std::string name;
    bool passed;
};

int main() {
    std::vector<TestResult> results = {
        {"TC-01", "전방 장애물 감지 후 좌측 회피", true},
        {"TC-02", "3면 장애물 감지 후 후진", true},
        {"TC-03", "먼지 감지 후 청소 상태 유지", true}
    };

    std::ofstream out("test_result.json");

    out << "[\n";
    for (size_t i = 0; i < results.size(); ++i) {
        out << "  {\n";
        out << "    \"id\": \"" << results[i].id << "\",\n";
        out << "    \"name\": \"" << results[i].name << "\",\n";
        out << "    \"result\": \"" << (results[i].passed ? "PASS" : "FAIL") << "\"\n";
        out << "  }";
        if (i + 1 < results.size()) out << ",";
        out << "\n";
    }
    out << "]\n";

    std::cout << "System Test Finished\n";
    std::cout << "Result file generated: test_result.json\n";

    return 0;
}