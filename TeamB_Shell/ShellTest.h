#pragma once
#include <algorithm>
#include <cctype>
#include <fstream>
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
  bool excuteRead(std::istringstream &iss);
  std::string getOutput();
  std::string readFromFile(const std::string &file);
  const std::string write_done = "[Write] Done\n";
  const std::string read_done = "[Read] ";
  const std::string invalid_command = "INVALID COMMAND\n";

 private:
  bool checkValidArgument(std::string &trashStr);
  bool checkValidLba(std::string &lbaStr);
  bool checkValidValue(std::string &valueStr);
  const std::string output_file_name = "ssd_output.txt";
};