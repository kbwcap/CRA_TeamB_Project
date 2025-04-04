#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "define.h"

class Logger {
 public:
  void print(std::string funcName, std::string message);

 private:
  void spiltLogFile();
  void compressOldLogFiles();
};
