#include "gmock/gmock.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>  // for stringstream
#include <iomanip>  // for hex formatting
#include <cctype> 
#include <algorithm>
#include "VirtualSSD.cpp"
#include <cstdint>

bool isValidHex(const char* str) {
  if (strncmp(str, "0x", 2) != 0) {
    return false;
  }

  size_t len = strlen(str);
  if (len != 10) {
    return false;
  }

  for (size_t i = 2; i < len; ++i) {
    if (!isxdigit(str[i])) {
      return false;
    }
  }

  return true;
}

int main(int argc, char* argv[]) {
#ifdef _DEBUG
  testing::InitGoogleTest();
  return RUN_ALL_TESTS();

#else
   VirtualSSD ssd;

  if (argc < 2 || argc > 4) {
    std::cerr << "Usage: ssd.exe W <int> <hexadecimal>  or  ssd.exe R <int>" << std::endl;
    return 1;
  }

  char c = argv[1][0];
  int num = std::atoi(argv[2]);

  if (c == 'W') {
    if (argc != 4) {
      std::cerr << "For 'W' mode, provide 2 arguments: <int> <hexadecimal>" << std::endl;
      return 1;
    }

    if (!isValidHex(argv[3])) {
      std::cerr << "Invalid input. Please enter a valid 8-digit hexadecimal value in the form 0xXXXXXXXX.\n";
      return 1;
    }

    uint32_t hexValue = std::strtoul(argv[3], nullptr, 16);

    std::cout << "Write Mode: num = " << num << ", hexValue = 0x" << std::hex << hexValue << std::endl;

    ssd.executeCommand(std::make_shared<WriteCommand>(ssd, num, hexValue));

  } else if (c == 'R') {
    if (argc != 3) {
      std::cerr << "For 'R' mode, provide 1 argument: <int>" << std::endl;
      return 1;
    }
    ssd.executeCommand(std::make_shared<ReadCommand>(ssd, num));
  }
  else {
    std::cerr << "Invalid mode. Use 'W' for write or 'R' for read." << std::endl;
    return 1;
  }
#endif
  return 0;
}