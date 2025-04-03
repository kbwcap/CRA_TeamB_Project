#pragma once
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
  TestManager() = default;
  void registerTest(const string& name, TestFn func);
  bool runTest(const string& name);
  vector<string> listTests();

 private:
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
