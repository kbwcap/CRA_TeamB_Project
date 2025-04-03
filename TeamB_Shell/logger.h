#pragma once

#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

#include "define.h"

class Logger {
 public:
  void print(std::string funcName, std::string message);
  void manageLog();
};
