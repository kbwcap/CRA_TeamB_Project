#pragma once
#include <string>
using std::string;


// ======================
// 인터페이스 정의
// ======================

class IShell {
public:
    virtual void executeCommand(const string& cmdLine) = 0;
    virtual string getOutput() = 0;
};