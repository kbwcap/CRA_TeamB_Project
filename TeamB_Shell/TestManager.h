#pragma once
#include <string>
using std::string;


// ======================
// �������̽� ����
// ======================

class IShell {
public:
    virtual void executeCommand(const string& cmdLine) = 0;
    virtual string getOutput() = 0;
};