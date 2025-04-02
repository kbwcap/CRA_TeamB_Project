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

typedef string (*TestFn)();

class TestManager {
public:
    void registerTest(const string& name, TestFn func);
    string runTest(const string& name);
    vector<string> listTests();

private:
    map<string, TestFn> testCases;
};

// lib
string toHexString(unsigned int value);
string getExpectedReadValue(unsigned int LBA, unsigned int expectedData);

// ======================
// 테스트 케이스
// ======================

string Test_FullWriteAndReadCompare_1();