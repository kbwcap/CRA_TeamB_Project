#pragma once

#include "TestManager.h"

#include <ctime>
#include <iostream>

#include "MockShell.h"
#include "UserCommandQueue.h"
#include "gmock/gmock.h"
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Sequence;
using ::testing::Test;

void TestManager::registerTest(const string& name, TestFn func) {
  testCases[name] = func;
}

int TestManager::runTest(const string& name) {
  if (testCases.count(name)) {
    return (testCases[name]()) ? PASS : FAIL;
  }

  // 없으면 prefix 매칭 탐색
  for (const auto& pair : testCases) {
    if (pair.first.rfind(name, 0) == 0) {
      return (pair.second()) ? PASS : FAIL;
    }
  }

  return NO_TC;
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

bool TestMock_FullWriteAndReadCompare_1() {
  NiceMock<MockShell> mockShell;
  UserCommandQueueMock queue(mockShell);

  const int minLba = 0;
  const int maxLba = 99;
  const unsigned int groupSize = 5;

  uint32_t seed = static_cast<uint32_t>(time(0));
  for (int base = minLba; base <= maxLba; base += groupSize + 1) {
    unsigned int pattern = patternGenerator(seed);

    for (unsigned int offset = 0; offset < groupSize && base + offset <= maxLba;
         ++offset) {
      if (!queue.enqueueWrite(base + offset, pattern)) return false;
    }
    for (unsigned int offset = 0; offset < groupSize && base + offset <= maxLba;
         ++offset) {
      if (!queue.enqueueRead(base + offset, pattern)) return false;
    }
  }

  queue.expectAll(mockShell);
  return queue.flush();
}

bool TestMock_PartialLBAWrite_2() {
  NiceMock<MockShell> mockShell;
  UserCommandQueueMock queue(mockShell);

  const std::vector<int> lbaOrder = {4, 0, 3, 1, 2};
  const int loopCount = 30;
  uint32_t seed = static_cast<uint32_t>(time(0));
  for (int loop = 0; loop < loopCount; ++loop) {
    unsigned int pattern = patternGenerator(seed);

    // Write sequence
    for (int lba : lbaOrder) {
      if (!queue.enqueueWrite(lba, pattern)) return false;
    }

    // ReadCompare sequence
    for (int lba : lbaOrder) {
      if (!queue.enqueueRead(lba, pattern)) return false;
    }
  }

  queue.expectAll(mockShell);
  return queue.flush();
}

bool TestMock_WriteReadAging_3() {
  NiceMock<MockShell> mockShell;
  UserCommandQueueMock queue(mockShell);

  const int maxLoopCount = 200;
  unsigned int pat1, pat2;

  uint32_t seed = static_cast<uint32_t>(time(0));
  for (int loop = 0; loop < maxLoopCount; loop++) {
    pat1 = patternGenerator(seed);
    pat2 = patternGenerator(seed);

    if (!queue.enqueueWrite(0, pat1)) return false;
    if (!queue.enqueueWrite(99, pat2)) return false;

    if (!queue.enqueueRead(0, pat1)) return false;
    if (!queue.enqueueRead(99, pat2)) return false;

    queue.expectAll(mockShell);
    if (!queue.flush()) return false;
  }

  queue.expectAll(mockShell);
  return queue.flush();
}

bool TestMock_EraseAndWriteAging_4() {
  NiceMock<MockShell> mockShell;
  UserCommandQueueMock queue(mockShell);

  const int maxLoopCount = 30;
  const int len = 2;
  const int maxLba = 99;
  unsigned int pat;

  uint32_t seed = static_cast<uint32_t>(time(0));
  for (int loop = 0; loop < maxLoopCount; loop++) {
    if (!queue.enqueueEraseRange(0, len)) return false;
    for (int startLba = len; startLba + len <= maxLba; startLba += len) {
      pat = patternGenerator(seed);
      if (!queue.enqueueWrite(startLba, pat)) return false;

      queue.expectAll(mockShell);
      if (!queue.flush()) return false;

      pat = patternGenerator(seed);
      if (!queue.enqueueWrite(startLba, pat)) return false;
      if (!queue.enqueueEraseRange(startLba, startLba + len)) return false;
    }
  }

  queue.expectAll(mockShell);
  return queue.flush();
}

bool TestMock_FullWriteFullReadFlush_5() {
  NiceMock<MockShell> mockShell;
  UserCommandQueueMock queue(mockShell);
  unsigned int pat;

  uint32_t seed = static_cast<uint32_t>(time(0));
  pat = patternGenerator(seed);
  if (!queue.enqueueFullWrite(pat)) return false;
  if (!queue.enqueueFullRead()) return false;
  if (!queue.enqueueFlush()) return false;
  queue.expectAll(mockShell);
  return queue.flush();
}

class TestScriptFixture : public Test {
 public:
  void registerAllTestcases() {
    testManager.registerTest("1_FullWriteAndReadCompare_Mock",
                             TestMock_FullWriteAndReadCompare_1);
    testManager.registerTest("2_PartialLBAWrite_Mock",
                             TestMock_PartialLBAWrite_2);
    testManager.registerTest("3_WriteReadAging_Mock",
                             TestMock_WriteReadAging_3);
    testManager.registerTest("4_EraseAndWriteAging_Mock",
                             TestMock_EraseAndWriteAging_4);
    testManager.registerTest("5_FullWriteFullReadFlush_Mock",
                             TestMock_FullWriteFullReadFlush_5);
  }

  void runTestcase(const std::string& testName) {
    int result = testManager.runTest(testName);
    std::cout << "[Test: " << testName << "] " << result << std::endl;
    EXPECT_EQ(result, testManager.PASS);
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
    int result = testManager.runTest(strNum + "_");
    std::cout << "[Test: " << testName << "] " << result << std::endl;
    EXPECT_EQ(result, testManager.PASS);
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

bool Test_FullWriteAndReadCompare_1() {
  ShellTest realShell;
  UserCommandQueue queue(realShell);

  const int minLba = 0;
  const int maxLba = 99;
  const unsigned int groupSize = 5;

  uint32_t seed = static_cast<uint32_t>(time(0));
  for (int base = minLba; base <= maxLba; base += groupSize + 1) {
    unsigned int pattern = patternGenerator(seed);

    for (unsigned int offset = 0; offset < groupSize && base + offset <= maxLba;
         ++offset) {
      if (!queue.enqueueWrite(base + offset, pattern)) return false;
    }
    for (unsigned int offset = 0; offset < groupSize && base + offset <= maxLba;
         ++offset) {
      if (!queue.enqueueRead(base + offset, pattern)) return false;
    }

    if (!queue.flush()) return false;
  }
  return true;
}

bool Test_PartialLBAWrite_2() {
  ShellTest realShell;
  UserCommandQueue queue(realShell);

  const std::vector<int> lbaOrder = {4, 0, 3, 1, 2};
  const int loopCount = 30;
  uint32_t seed = static_cast<uint32_t>(time(0));
  for (int loop = 0; loop < loopCount; ++loop) {
    unsigned int pattern = patternGenerator(seed);

    // Write sequence
    for (int lba : lbaOrder) {
      if (!queue.enqueueWrite(lba, pattern)) return false;
    }

    // ReadCompare sequence
    for (int lba : lbaOrder) {
      if (!queue.enqueueRead(lba, pattern)) return false;
    }
  }

  return queue.flush();
}

bool Test_WriteReadAging_3() {
  ShellTest realShell;
  UserCommandQueue queue(realShell);

  const int maxLoopCount = 200;
  unsigned int pat1, pat2;

  uint32_t seed = static_cast<uint32_t>(time(0));
  for (int loop = 0; loop < maxLoopCount; loop++) {
    pat1 = patternGenerator(seed);
    pat2 = patternGenerator(seed);
    if (!queue.enqueueWrite(0, pat1)) return false;
    if (!queue.enqueueWrite(99, pat2)) return false;

    if (!queue.enqueueRead(0, pat1)) return false;
    if (!queue.enqueueRead(99, pat2)) return false;
  }

  return queue.flush();
}

bool Test_EraseAndWriteAging_4() {
  ShellTest realShell;
  UserCommandQueue queue(realShell);

  const int maxLoopCount = 30;
  const int len = 2;
  const int maxLba = 99;
  unsigned int pat;

  uint32_t seed = static_cast<uint32_t>(time(0));
  for (int loop = 0; loop < maxLoopCount; loop++) {
    if (!queue.enqueueEraseRange(0, len)) return false;
    for (int startLba = len; startLba + len <= maxLba; startLba += len) {
      pat = patternGenerator(seed);
      if (!queue.enqueueWrite(startLba, pat)) return false;

      pat = patternGenerator(seed);
      if (!queue.enqueueWrite(startLba, pat)) return false;
      if (!queue.enqueueEraseRange(startLba, startLba + len)) return false;
    }
  }

  return queue.flush();
}

bool Test_FullWriteFullReadFlush_5() {
  ShellTest realShell;
  UserCommandQueue queue(realShell);
  unsigned int pat;

  uint32_t seed = static_cast<uint32_t>(time(0));
  pat = patternGenerator(seed);
  if (!queue.enqueueFullWrite(pat)) return false;
  if (!queue.enqueueFullRead()) return false;
  if (!queue.enqueueFlush()) return false;

  return queue.flush();
}
