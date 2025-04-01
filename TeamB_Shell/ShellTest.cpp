#include "ShellTest.h"

void ShellTest::runShellTest(const std::string& input) {
  std::istringstream iss(input);
  std::string command;
  iss >> command;

  if (command == "write") {
    std::string lbaStr, valueStr, trashStr;
    iss >> lbaStr >> valueStr >> trashStr;

    if (trashStr.length() > 0) {
      std::cout << "INVALID COMMAND\n";
      return;
    }

    if (lbaStr.length() < 0 || lbaStr.length() >= 3) {
      std::cout << "INVALID COMMAND\n";
      return;
    }
    for (char lbaChar : lbaStr) {
      if (lbaChar < '0' || lbaChar > '9') {
        std::cout << "INVALID COMMAND\n";
        return;
      }
    }

    if (valueStr.length() != 10) {
      std::cout << "INVALID COMMAND\n";
      return;
    }
    // to upper case
    std::transform(valueStr.begin(), valueStr.end(), valueStr.begin(),
                   [](unsigned char c) { return std::toupper(c); });

    if (valueStr[0] != '0' || valueStr[1] != 'X') {
      std::cout << "INVALID COMMAND\n";
      return;
    }
    std::string valueStrSub = valueStr.substr(2);
    for (char valueChar : valueStrSub) {
      if ((valueChar < '0' || valueChar > '9') &&
          (valueChar < 'A' || valueChar > 'F')) {
        std::cout << "INVALID COMMAND\n";
        return;
      }
    }

    std::string newCommand = "ssd.exe W " + lbaStr + " " + valueStr;
    system(newCommand.c_str());
    return;
  } else if (command == "read") {
  } else if (command == "exit") {
  } else if (command == "help") {
  } else if (command == "fullwrite") {
  } else if (command == "fullread") {
  }
  std::cout << "INVALID COMMAND\n";
}