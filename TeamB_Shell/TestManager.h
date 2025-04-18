
#ifndef TESTMANAGER_H
#define TESTMANAGER_H
#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;

// ======================
// 테스트용 타입과 매니저
// ======================

typedef bool (*TestFn)();

class TestManager {
 public:
  static TestManager& instance();
  void clearTests();
  void loadTestSuitesDLL();
  void registerTest(const string& name, TestFn func);
  int runTest(const string& name);
  vector<string> listTests();
  const int FAIL = 0;
  const int PASS = 1;
  const int NO_TC = 2;

 private:
  TestManager() {};
  TestManager& operator=(const TestManager& other) = delete;
  TestManager(const TestManager& other) = delete;

  map<string, TestFn> testCases;
};

// lib
uint32_t patternGenerator(uint32_t& state);
string toHexString(unsigned int value);

// ======================
// 테스트 케이스
// ======================

bool TestMock_FullWriteAndReadCompare_1();
bool TestMock_PartialLBAWrite_2();
bool TestMock_WriteReadAging_3();
bool TestMock_EraseAndWriteAging_4();
bool TestMock_FullWriteFullReadFlush_5();

#endif
