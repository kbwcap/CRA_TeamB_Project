#pragma once
#include "TestManager.h"

#include <ctime>
#include <iostream>

#include "MockShell.h"
#include "gmock/gmock.h"
using ::testing::Return;
using ::testing::Sequence;
using ::testing::Test;

void TestManager::registerTest(const string& name, TestFn func) {
  testCases[name] = func;
}

bool TestManager::runTest(const string& name) {
  if (testCases.count(name)) return testCases[name]();

  // 없으면 prefix 매칭 탐색
  for (const auto& pair : testCases) {
    if (pair.first.rfind(name, 0) == 0) {
      return pair.second();
    }
  }

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

uint32_t patternGenerator(uint32_t& state) {
  state ^= state << 13;
  state ^= state >> 17;
  state ^= state << 5;
  return state;
}

string toHexString(unsigned int value) {
  std::ostringstream ss;
  ss << "0x" << std::uppercase  // 대문자 A~F
     << std::hex                // 16진수 출력
     << std::setfill('0')       // 빈 자리는 0으로 채움
     << std::setw(8)            // 8자리 고정
     << value;
  return ss.str();
}

string getExpectedReadValue(int LBA, unsigned int expectedData) {
  return std::to_string(LBA) + " " + toHexString(expectedData);
}

bool readCompare(int LBA, unsigned int expectedData, MockShell& mockShell) {
  string readInput = "r " + std::to_string(LBA);
  mockShell.executeCommand(readInput);

  return (mockShell.getOutput() == getExpectedReadValue(LBA, expectedData));
}

bool TestMock_FullWriteAndReadCompare_1() {
  MockShell mockShell;
  const int minLba = 0;
  const int maxLba = 99;
  const unsigned int step = 5;

  unsigned int pat;
  vector<unsigned int> patList;
  vector<string> writeInput;
  vector<string> readInput;
  vector<string> expectedOutputs;
  Sequence s;

  uint32_t seed = static_cast<uint32_t>(time(0));
  // generate input for writedata, readdata, and expected result
  for (int lba = minLba; lba <= maxLba; lba++) {
    if (lba % step == 0) {
      pat = patternGenerator(seed);
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
  for (int lba = minLba; lba <= maxLba;) {
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

bool TestMock_PartialLBAWrite_2() {
  MockShell mockShell;
  vector<string> writeInput;
  vector<string> readInput;
  vector<string> expectedOutputs;
  unsigned int pat;
  vector<unsigned int> patList;
  vector<int> lbaList = {4, 0, 3, 1, 2};
  Sequence expectedOutputSeq;
  Sequence readInputSeq;
  const int maxLoopCount = 30;
  uint32_t seed = static_cast<uint32_t>(time(0));
  for (int loopCount = 0; loopCount < maxLoopCount; loopCount++) {
    pat = patternGenerator(seed);
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
  for (int loopCount = 0; loopCount < maxLoopCount; loopCount++) {
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

bool TestMock_WriteReadAging_3() {
  MockShell mockShell;

  const int maxLoopCount = 200;
  const int maxInterval = 50;
  vector<string> writeInput;
  vector<string> readInput;
  vector<string> expectedOutputs;
  vector<unsigned int> patList;
  unsigned int patFristWrite;
  unsigned int patSecondWrite;
  uint32_t seed = static_cast<uint32_t>(time(0));
  string wInput, rInput;
  for (int loopCount = 0; loopCount < maxLoopCount; loopCount += maxInterval) {
    patList.clear();
    writeInput.clear();
    readInput.clear();
    expectedOutputs.clear();
    Sequence expectedOutputSeq;
    Sequence writeInputSeq;
    Sequence readInputSeq;
    for (int interval = 0; interval < maxInterval; interval++) {
      // prepare input / output / expected result
      patFristWrite = patternGenerator(seed);
      patList.push_back(patFristWrite);
      wInput = "w " + std::to_string(0) + " " + toHexString(patFristWrite);
      rInput = "r " + std::to_string(0);
      writeInput.push_back(wInput);
      readInput.push_back(rInput);
      expectedOutputs.push_back(getExpectedReadValue(0, patFristWrite));

      patSecondWrite = patternGenerator(seed);
      patList.push_back(patSecondWrite);
      wInput = "w " + std::to_string(99) + " " + toHexString(patSecondWrite);
      rInput = "r " + std::to_string(99);
      writeInput.push_back(wInput);
      readInput.push_back(rInput);
      expectedOutputs.push_back(getExpectedReadValue(99, patSecondWrite));
    }

    // expect call
    for (const auto& wInput : writeInput) {
      EXPECT_CALL(mockShell, executeCommand(wInput)).InSequence(writeInputSeq);
    }

    for (const auto& rInput : readInput) {
      EXPECT_CALL(mockShell, executeCommand(rInput)).InSequence(readInputSeq);
    }

    for (const auto& expected : expectedOutputs) {
      EXPECT_CALL(mockShell, getOutput())
          .InSequence(expectedOutputSeq)
          .WillOnce(Return(expected));
    }

    // test
    int writeCount = 0;
    int readCount = 0;
    int patCount = 0;
    for (int interval = 0; interval < maxInterval; interval++) {
      mockShell.executeCommand(writeInput[writeCount++]);
      mockShell.executeCommand(writeInput[writeCount++]);
      // readCompare
      if (readCompare(0, patList[patCount++], mockShell) == false) return false;
      if (readCompare(99, patList[patCount++], mockShell) == false)
        return false;
    }
  }

  return true;
}

class TestScriptFixture : public Test {
 public:
  void registerAllTestcases() {
    testManager.registerTest("1_FullWriteAndReadCompare_Mock",
                             TestMock_FullWriteAndReadCompare_1);
    testManager.registerTest("2_PartialLBAWrite_Mock", TestMock_PartialLBAWrite_2);
    testManager.registerTest("3_WriteReadAging_Mock", TestMock_WriteReadAging_3);
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

    int number;
    std::string strNum = testName.substr(0, pos);
    try {
      number = std::stoi(strNum);
    } catch (const std::exception& e) {
      std::cerr << "[Test: " << testName
                << "] Invalid number format: " << e.what() << std::endl;
      return;
    }
    bool result = testManager.runTest(strNum + "_");
    std::cout << "[Test: " << testName << "] " << result << std::endl;
    EXPECT_TRUE(result);
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