#pragma once
#include "TestManager.h"
#include "gmock/gmock.h"
#include <iostream>
using ::testing::Sequence;
using ::testing::Return;

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

string Test_FullWriteAndReadCompare_1() {
    MockShell mockShell;
    const unsigned int minLba = 0;
    const unsigned int maxLba = 99;
    const unsigned int step = 5;

    unsigned int pat;
    vector<unsigned int> patList;
    vector<string> writeInput;
    vector<string> readInput;
    vector<string> expectedOutputs;
    Sequence s;

    // generate input for writedata, readdata, and expected result
    for (unsigned int lba = minLba; lba <= maxLba; lba++) {
        if (lba % step == 0) {
            pat = rand() % 0xFFFFFFFF + 1;
            patList.push_back(pat);
        }

        string wInput = "w " + std::to_string(lba) + " " + toHexString(pat);
        string rInput = "r " + std::to_string(lba);
        writeInput.push_back(wInput);
        readInput.push_back(rInput);
        expectedOutputs.push_back(getExpectedReadValue(lba, pat));
    }

    // Expect Call
    for (const auto& wInput : writeInput) {
        EXPECT_CALL(mockShell, executeCommand(wInput)).Times(1);
    }

    for (const auto& rInput : readInput) {
        EXPECT_CALL(mockShell, executeCommand(rInput)).Times(1);
    }

    for (const auto& expected : expectedOutputs) {
        EXPECT_CALL(mockShell, getOutput())
            .InSequence(s)
            .WillOnce(Return(expected));
    }

    // Verify
    int patCount = 0;
    int writeCount = 0;
    int readCount = 0;
    for (unsigned int lba = minLba; lba <= maxLba;) {
        if (lba % step == 0) {
            pat = patList[patCount++];
        }
        for (unsigned int index = 0; index < step; index++) {
            mockShell.executeCommand(writeInput[writeCount++]);
        }
        // readCompare
        for (unsigned int index = 0; index < step; index++, lba++) {
            if (readCompare(lba, pat, mockShell) == false) return "FAILED";
        }
    }

    return "PASSED";
}

// TestScripts
TEST(TestScripts, RunTestAll) {
    TestManager testManager;
    vector<string> tests = testManager.listTests();
    testManager.registerTest("1_FullWriteAndReadCompare", Test_FullWriteAndReadCompare_1);

    for (int testIndex = 0; testIndex < tests.size(); ++testIndex) {
        string result = testManager.runTest(tests[testIndex]);
        std::cout << "[Test: " << tests[testIndex] << "] " << result << std::endl;
        EXPECT_TRUE(result == "PASSED");
    }
}

TEST(TestScripts, PrefixTest){
    TestManager testManager;
    testManager.registerTest("1_FullWriteAndReadCompare", Test_FullWriteAndReadCompare_1);
    vector<string> tests = testManager.listTests();
    string result = testManager.runTest("1_");
    std::cout << "[Test: " << tests[0] << "] " << result << std::endl;
    EXPECT_TRUE(result == "PASSED");
}