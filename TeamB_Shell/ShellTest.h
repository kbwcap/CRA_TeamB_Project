#pragma once
#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <sstream>

class ShellTest {
 public:
  void runShellTest(const std::string &input);
  bool excuteWrite(std::istringstream &iss);

 private:
  bool checkValidArgument(std::string &trashStr);
  bool checkValidLba(std::string &lbaStr);
  bool checkValidValue(std::string &valueStr);
};