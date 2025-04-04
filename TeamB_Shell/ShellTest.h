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
  void executeCommand(const std::string &cmdLine);
  std::string getOutput();
  std::string readFromFile(const std::string &file);

 private:
  bool executeWrite(std::istringstream &iss);
  bool executeRead(std::istringstream &iss);
  void printHelp();
  bool executeFullWrite(std::istringstream &iss);
  bool executeFullRead(std::istringstream &iss);
  bool executeErase(std::istringstream &iss);
  bool executeEraseRange(std::istringstream &iss);
  bool executeFlush(std::istringstream &iss);
  void runner();
  void sendEraseCommand(long long sizeNum, int lbaNum);
  bool checkValidArgument(std::string &trashStr);
  bool checkValidLba(std::string &lbaStr);
  bool checkValidValue(std::string &valueStr);
  bool checkValidSize(std::string &valueStr);
};