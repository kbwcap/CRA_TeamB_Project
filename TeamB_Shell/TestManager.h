#pragma once
#include <string>
#include <vector>
#include <map>
using std::string;
using std::vector;
using std::map;

// ======================
// �������̽� ����
// ======================

class IShell {
public:
    virtual void executeCommand(const string& cmdLine) = 0;
    virtual string getOutput() = 0;
};

// ======================
// �׽�Ʈ�� Ÿ�԰� �Ŵ���
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