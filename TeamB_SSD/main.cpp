#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include "VirtualSSD.h"
#include "WriteCommand.h"
#include "ReadCommand.h"
#include "FlushCommand.h"
#include "EraseCommand.h"
#include "gmock/gmock.h"

bool isValidHex(const char* str) {
  if (!strncmp(str, "0X", 2)) {
    return false;
  }

  if (strncmp(str, "0x", 2)) {
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

void showUsage() {
  std::cerr << "Usage: ssd.exe W <int> <hexadecimal>  or  ssd.exe R <int>"
            << std::endl;
}

bool processWrite(int num, const std::string& hexStr, VirtualSSD& ssd) {
  if (!isValidHex(hexStr.c_str())) {
    std::cerr << "Invalid hexadecimal input. Please enter a valid 8-digit "
                 "value in the form 0xXXXXXXXX."
              << std::endl;
    return false;
  }

  uint32_t hexValue = std::strtoul(hexStr.c_str(), nullptr, 16);
  return ssd.executeCommand(std::make_shared<WriteCommand>(ssd, num, hexValue));
}

bool processRead(int num, VirtualSSD& ssd) {
  return ssd.executeCommand(std::make_shared<ReadCommand>(ssd, num));
}

bool flushRead(VirtualSSD& ssd) {
  return ssd.executeCommand(std::make_shared<FlushCommand>(ssd));
}

bool processErase(int num, int size, VirtualSSD& ssd) {
  return ssd.executeCommand(std::make_shared<EraseCommand>(ssd, num, size));
}

bool parseArguments(int argc, char* argv[], char& mode, int& num,
                    std::string& hexStr, int& size) {
  if (argc < 2 || argc > 4) {
    showUsage();
    return false;
  }

  mode = argv[1][0];
  num = std::atoi(argv[2]);

  if (mode == 'W') {
    if (argc != 4) {
      std::cerr << "For 'W' mode, provide 2 arguments: <int> <hexadecimal>"
                << std::endl;
      return false;
    }
    hexStr = argv[3];
  } else if (mode == 'R') {
    if (argc != 3) {
      std::cerr << "For 'R' mode, provide 1 argument: <int>" << std::endl;
      return false;
    }
  } else if (mode == 'F') {
    if (argc != 1) {
      std::cerr << "For 'F' mode, require no argument" << std::endl;
      return false;
    }
  } else if (mode == 'E') {
    if (argc != 4) {
      std::cerr << "For 'E' mode, provide 1 argument: <int>" << std::endl;
      return false;
    }
    size = std::atoi(argv[3]);
  } else {
    std::cerr << "Invalid mode. Use 'W' for write or 'R' for read."
              << std::endl;
    return false;
  }

  return true;
}

int main(int argc, char* argv[]) {
#ifdef _DEBUG
  testing::InitGoogleTest();
  return RUN_ALL_TESTS();
#else
  VirtualSSD ssd;
  char mode;
  int num;
  std::string hexStr;
  int size;

  if (!parseArguments(argc, argv, mode, num, hexStr, size)) {
    return 1;
  }

  if (mode == 'W') {
    return processWrite(num, hexStr, ssd) ? 0 : 1;
  } else if (mode == 'R') {
    return processRead(num, ssd);
  } else if (mode == 'F') {
    return flushRead(ssd);
  } else if (mode == 'E') {
    processErase(num, size, ssd);
    return 0;
  }

#endif
}
