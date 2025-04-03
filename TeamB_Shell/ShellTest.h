#pragma once
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "TestManager.h"
#include "define.h"

class IShell {
 public:
  virtual void executeCommand(const std::string &cmdLine) = 0;
  virtual std::string getOutput() = 0;
};

class ShellTest : public IShell {
 public:
  ShellTest() {}
  ShellTest(TestManager *tm) : testManager(tm) {}
  void executeCommand(const std::string &cmdLine);
  std::string getOutput();
  std::string readFromFile(const std::string &file);

 private:
  void printHelp();
  bool excuteWrite(std::istringstream &iss);
  bool excuteRead(std::istringstream &iss);
  bool excuteFullWrite(std::istringstream &iss);
  bool excuteFullRead(std::istringstream &iss);
  bool excuteErase(std::istringstream &iss);
  bool excuteFlush(std::istringstream &iss);
  bool checkValidArgument(std::string &trashStr);
  bool checkValidLba(std::string &lbaStr);
  bool checkValidValue(std::string &valueStr);
  bool checkValidSize(std::string &valueStr);

  TestManager *testManager;
};