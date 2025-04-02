#pragma once
#include "TestManager.h"
#include <iostream>
#include "ShellTest.h"
#include "gmock/gmock.h"
using ::testing::Return;
using ::testing::Sequence;
using ::testing::Test;

class MockShell : public IShell {
 public:
  MOCK_METHOD(void, executeCommand, (const std::string& cmdLine), (override));
  MOCK_METHOD(std::string, getOutput, (), (override));
};

void TestManager::registerTest(const string& name, TestFn func) {
  testCases[name] = func;
}

bool TestManager::runTest(const string& name) {
  // 정확히 일치하는 테스트가 있다면 먼저 실행
  if (testCases.count(name)) return testCases[name]();

  // 없으면 prefix 매칭 탐색
  for (const auto& pair : testCases) {
    if (pair.first.rfind(name, 0) == 0) {  // key가 name으로 시작하는지 체크
      return pair.second();
    }
  }

  // Test Shell에서 write, read, exit, help, fullwrite, fullread 이외에 다른
  // 문자가 들어올 경우
  return false;
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

bool readCompare(unsigned int LBA, unsigned int expectedData,
                 MockShell& mockShell) {
  string readInput = "r " + std::to_string(LBA);
  mockShell.executeCommand(readInput);

  return (mockShell.getOutput() == getExpectedReadValue(LBA, expectedData));
}

bool Test_FullWriteAndReadCompare_1() {
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
      if (readCompare(lba, pat, mockShell) == false) return false;
    }
  }
  return true;
}

bool Test_PartialLBAWrite_2() {
  MockShell mockShell;
  vector<string> writeInput;
  vector<string> readInput;
  vector<string> expectedOutputs;
  unsigned int pat;
  vector<unsigned int> patList;
  vector<unsigned int> lbaList = {4, 0, 3, 1, 2};
  Sequence expectedOutputSeq;
  Sequence readInputSeq;

  for (int loopCount = 0; loopCount < 30; loopCount++) {
    pat = rand() % 0xFFFFFFFF + 1;
    patList.push_back(pat);
    for (int count = 0; count < 5; count++) {
      string wInput =
          "w " + std::to_string(lbaList[count]) + " " + toHexString(pat);
      string rInput = "r " + std::to_string(lbaList[count]);
      writeInput.push_back(wInput);
      readInput.push_back(rInput);
      expectedOutputs.push_back(getExpectedReadValue(lbaList[count], pat));
    }
  }

  // Expect Call
  for (const auto& wInput : writeInput) {
    EXPECT_CALL(mockShell, executeCommand(wInput)).Times(1);
  }

  for (const auto& rInput : readInput) {
    EXPECT_CALL(mockShell, executeCommand(rInput)).InSequence(readInputSeq);
  }

  for (const auto& expected : expectedOutputs) {
    EXPECT_CALL(mockShell, getOutput())
        .InSequence(expectedOutputSeq)
        .WillOnce(Return(expected));
  }

  // Test
  int writeCount = 0;
  int readCount = 0;
  for (int loopCount = 0; loopCount < 30; loopCount++) {
    pat = patList[loopCount];
    for (int count = 0; count < 5; count++) {
      mockShell.executeCommand(writeInput[writeCount++]);
    }

    // readCompare
    for (int count = 0; count < 5; count++) {
      if (readCompare(lbaList[count], pat, mockShell) == false) return false;
    }
  }

  return true;
}

class TestScriptFixture : public Test {
 public:
  void registerAllTestcases() {
    testManager.registerTest("1_FullWriteAndReadCompare",
                             Test_FullWriteAndReadCompare_1);
    testManager.registerTest("2_PartialLBAWrite",
                             Test_PartialLBAWrite_2);
  }

  void runTestcase(const std::string& testName) {
    bool result = testManager.runTest(testName);
    std::cout << "[Test: " << testName << "] " << result << std::endl;
    EXPECT_TRUE(result);
  }

  void runPrefixTestcase(const std::string& testName) {
    size_t pos = testName.rfind('_');

    if (pos == std::string::npos || pos <= 0) {
      std::cout << "Need to follow TC Name Rule\n";
      return;
    }

    try {
      std::string strNum = testName.substr(0, pos);
      int number = std::stoi(strNum);
      bool result = testManager.runTest(strNum + "_");
      std::cout << "[Test: " << testName << "] " << result << std::endl;
      EXPECT_TRUE(result);
    } catch (const std::invalid_argument& e) {
      std::cout << "there is not Number\n";
    }
  }

  void runAllTestcases() {
    vector<string> tests = testManager.listTests();
    for (const auto& test : tests) {
      runTestcase(test);
    }
  }

  void runAllPrefixTestcases() {
    vector<string> tests = testManager.listTests();
    for (const auto& test : tests) {
      runPrefixTestcase(test);
    }
  }

 private:
  TestManager testManager;
};

// TestScripts
TEST_F(TestScriptFixture, RunTestAll) {
  registerAllTestcases();
  runAllTestcases();
}

TEST_F(TestScriptFixture, PrefixTest) {
  registerAllTestcases();
  runAllPrefixTestcases();
}