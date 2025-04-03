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
  std::string getOutput();
  std::string readFromFile(const std::string &file);
  const std::string write_done = "[Write] Done\n";
  const std::string read_done = "[Read] ";
  const std::string fullwrite_done = "[FullWrite] Done\n";
  const std::string invalid_command = "INVALID COMMAND\n";
  const std::string flush_done = "[Flush] Done\n";
  const std::string err_too_big_size = "[Error] Size is too big\n";
  const int MAX_LBA = 100;
  const int MAX_SIZE = 10;

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
  const std::string output_file_name = "ssd_output.txt";
};