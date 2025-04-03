#pragma once
#include "ShellTest.h"
#include "gmock/gmock.h"

class MockShell : public IShell {
 public:
  MOCK_METHOD(void, executeCommand, (const std::string& cmdLine), (override));
  MOCK_METHOD(std::string, getOutput, (), (override));
};
