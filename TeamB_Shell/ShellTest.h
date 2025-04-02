#pragma once
#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

class IShell {
 public:
  virtual void executeCommand(const std::string &cmdLine) = 0;
  virtual std::string getOutput() = 0;
};

class ShellTest : public IShell {
 public:
  void executeCommand(const std::string &cmdLine);
  bool excuteWrite(std::istringstream &iss);
  std::string getOutput();

 private:
  bool checkValidArgument(std::string &trashStr);
  bool checkValidLba(std::string &lbaStr);
  bool checkValidValue(std::string &valueStr);
};