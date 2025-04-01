#pragma once
#include "TestManager.h"
#include "gmock/gmock.h"
#include <iostream>

class MockShell : public IShell {
public:
    MOCK_METHOD(void, executeCommand, (const std::string& cmdLine), (override));
    MOCK_METHOD(std::string, getOutput, (), (override));
};