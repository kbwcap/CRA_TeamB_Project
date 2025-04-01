#pragma once
#include "TestManager.h"
#include "gmock/gmock.h"
#include <iostream>

class MockShell : public IShell {
public:
    MOCK_METHOD(void, executeCommand, (const std::string& cmdLine), (override));
    MOCK_METHOD(std::string, getOutput, (), (override));
};

void TestManager::registerTest(const string& name, TestFn func) {
    testCases[name] = func;
}

string TestManager::runTest(const string& name) {
    // 정확히 일치하는 테스트가 있다면 먼저 실행
    if (testCases.count(name)) return testCases[name]();

    // 없으면 prefix 매칭 탐색
    for (const auto& pair : testCases) {
        if (pair.first.rfind(name, 0) == 0) {  // key가 name으로 시작하는지 체크
            return pair.second();
        }
    }

    return "Unknown test case: " + name;
}

vector<string> TestManager::listTests() {
    vector<string> names;
    for (const auto& it : testCases) {
        names.push_back(it.first);
    }
    return names;
}

// ======================
// ReadCompare
// ======================

string toHexString(unsigned int value) {
    std::ostringstream ss;
    ss << "0x" << std::uppercase  // 대문자 A~F
        << std::hex                // 16진수 출력
        << std::setfill('0')       // 빈 자리는 0으로 채움
        << std::setw(8)            // 8자리 고정
        << value;
    return ss.str();
}

string getExpectedReadValue(unsigned int LBA, unsigned int expectedData) {
    return std::to_string(LBA) + " " + toHexString(expectedData);
}

bool readCompare(unsigned int LBA, unsigned int expectedData, MockShell& mockShell) {
    string readInput = "r " + std::to_string(LBA);
    mockShell.executeCommand(readInput);

    return (mockShell.getOutput() == getExpectedReadValue(LBA, expectedData));
}

// TestScripts
TEST(TestScripts, RunTestAll) {
    TestManager testManager;
    vector<string> tests = testManager.listTests();
    for (int testIndex = 0; testIndex < tests.size(); ++testIndex) {
        string result = testManager.runTest(tests[testIndex]);
        std::cout << "[Test: " << tests[testIndex] << "] " << result << std::endl;
        EXPECT_TRUE(result == "PASSED");
    }
}