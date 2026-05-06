#include "test_common.h"
#include "test_positive.h"
#include "test_negative.h"

int main() {
    std::vector<TestResult> results;

    std::cout << "\n[Positive Test - 01]\n";
    addResult(results, "TC-POS-01", "Positive", "UC-00 -> UC-01 좌측 회피 전체 흐름", TC_POS_01);

    std::cout << "\n[Positive Test - 02]\n";
    addResult(results, "TC-POS-02", "Positive", "UC-00 -> UC-03 고먼지 집중 청소", TC_POS_02);

    std::cout << "\n[Positive Test - 03]\n";
    addResult(results, "TC-POS-03", "Positive", "UC-00 -> UC-01 -> UC-02 모터 시퀀스(왼쪽)", TC_POS_03);

    std::cout << "\n[Positive Test - 04]\n";
    addResult(results, "TC-POS-04", "Positive", "UC-00 -> UC-01 -> UC-02 모터 시퀀스(왼쪽 막히고 오른쪽)", TC_POS_04);

    std::cout << "\n[Positive Test - 05]\n";
    addResult(results, "TC-POS-05", "Positive", "UC-03 -> UC-01 집중 청소 후 회피", TC_POS_05);

    std::cout << "\n[Positive Test - 06]\n";
    addResult(results, "TC-POS-06", "Positive", "2회 연속 전방 장애물 회피", TC_POS_06);

    std::cout << "\n[Positive Test - 07]\n";
    addResult(results, "TC-POS-07", "Positive", "UC-03 2회 연속 집중 청소", TC_POS_07);

    std::cout << "\n[Positive Test - 08]\n";
    addResult(results, "TC-POS-08", "Positive", "UC-00 -> UC-03 미진입 (저먼지 조건) 일반 청소 유지", TC_POS_08);

    std::cout << "\n[Positive Test - 09]\n";
    addResult(results, "TC-POS-09", "Positive", "UC-00 -> UC-03 -> UC-01 -> UC-02 전체 순차 수행", TC_POS_09);

    std::cout << "\n[Positive Test - 10]\n";
    addResult(results, "TC-POS-10", "Positive", "UC-00 -> UC-01 미진입 (좌우 장애물, 전방 개방) 직진 유지", TC_POS_10);

    std::cout << "\n[Negative Test - 01]\n";
    addResult(results, "TC-NEG-01", "Negative", "CLEANING 중 startCleaning 중복 호출", TC_NEG_01);

    std::cout << "\n[Negative Test - 02]\n";
    addResult(results, "TC-NEG-02", "Negative", "CLEANING 중 resumeCleaning 호출", TC_NEG_02);

    std::cout << "\n[Negative Test - 03]\n";
    addResult(results, "TC-NEG-03", "Negative", "유효하지 않은 SystemState 변경 시도", TC_NEG_03);

    std::cout << "\n[Negative Test - 04]\n";
    addResult(results, "TC-NEG-04", "Negative", "AVOIDING 중 startCleaning 호출", TC_NEG_04);

    std::cout << "\n[Negative Test - 05]\n";
    addResult(results, "TC-NEG-05", "Negative", "동일한 상태로 changeState 호출", TC_NEG_05);

    std::cout << "\n[Negative Test - 06]\n";
    addResult(results, "TC-NEG-06", "Negative", "비정상 Position 데이터 입력", TC_NEG_06);

    std::cout << "\n[Negative Test - 07]\n";
    addResult(results, "TC-NEG-07", "Negative", "회피 중 전방 장애물 재발생", TC_NEG_07);

    std::cout << "\n[Negative Test - 08]\n";
    addResult(results, "TC-NEG-08", "Negative", "FORWARD 상태에서 moveForward 중복 호출", TC_NEG_08);

    std::cout << "\n[Negative Test - 09]\n";
    addResult(results, "TC-NEG-09", "Negative", "BACKWARD 상태에서 moveBackward 중복 호출", TC_NEG_09);

    std::cout << "\n[Negative Test - 10]\n";
    addResult(results, "TC-NEG-10", "Negative", "LEFT 상태에서 turnLeft 중복 호출", TC_NEG_10);

    std::cout << "\n[Negative Test - 11]\n";
    addResult(results, "TC-NEG-11", "Negative", "RIGHT 상태에서 turnRight 중복 호출", TC_NEG_11);

    std::cout << "\n[Negative Test - 12]\n";
    addResult(results, "TC-NEG-12", "Negative", "STOPPED 상태에서 stopMotor 중복 호출", TC_NEG_12);

    std::cout << "\n[Negative Test - 13]\n";
    addResult(results, "TC-NEG-13", "Negative", "음수 DustLevel 입력", TC_NEG_13);

    std::cout << "\n[Negative Test - 14]\n";
    addResult(results, "TC-NEG-14", "Negative", "매우 큰 DustLevel 입력", TC_NEG_14);

    std::cout << "\n[Negative Test - 15]\n";
    addResult(results, "TC-NEG-15", "Negative", "OFF 상태에서 deactivateCleaner 호출", TC_NEG_15);

    std::cout << "\n[Negative Test - 16]\n";
    addResult(results, "TC-NEG-16", "Negative", "NORMAL 상태에서 activateCleaner 중복 호출", TC_NEG_16);

    std::cout << "\n[Negative Test - 17]\n";
    addResult(results, "TC-NEG-17", "Negative", "BOOST 상태에서 boostPower 중복 호출", TC_NEG_17);

    std::cout << "\n[Negative Test - 18]\n";
    addResult(results, "TC-NEG-18", "Negative", "NORMAL 상태에서 normalizePower 중복 호출", TC_NEG_18);

    std::cout << "\n[Negative Test - 19]\n";
    addResult(results, "TC-NEG-19", "Negative", "OFF 상태에서 normalizePower 호출", TC_NEG_19);

    std::cout << "\n[Negative Test - 20]\n";
    addResult(results, "TC-NEG-20", "Negative", "OFF 상태에서 boostPower 호출", TC_NEG_20);

    std::ofstream out("test_result.json");

    out << "[\n";
    for (size_t i = 0; i < results.size(); ++i) {
        out << "  {\n";
        out << "    \"id\": \""     << results[i].id   << "\",\n";
        out << "    \"type\": \""   << results[i].type << "\",\n";
        out << "    \"name\": \""   << results[i].name << "\",\n";
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