#include "ShellTest.h"

void ShellTest::executeCommand(const std::string &input) {
  std::istringstream iss(input);
  std::string command;
  iss >> command;

  if (command == "write") {
    if (!excuteWrite(iss)) {
      std::cout << "INVALID COMMAND\n";
    } else
      std::cout << "[Write] Done\n";
    return;
  } else if (command == "read") {
    if (!excuteRead(iss)) {
      std::cout << "INVALID COMMAND\n";
    } else
      std::cout << "[Read] " + getOutput() + "\n";
    return;
  } else if (command == "exit") {
  } else if (command == "help") {
  } else if (command == "fullwrite") {
  } else if (command == "fullread") {
  }
  std::cout << "INVALID COMMAND\n";
}

bool ShellTest::excuteWrite(std::istringstream &iss) {
  std::string lbaStr, valueStr, trashStr;
  iss >> lbaStr >> valueStr >> trashStr;

  // to upper case
  std::transform(valueStr.begin(), valueStr.end(), valueStr.begin(),
                 [](unsigned char c) { return std::toupper(c); });

  if (!checkValidArgument(trashStr)) return false;
  if (!checkValidLba(lbaStr)) return false;
  if (!checkValidValue(valueStr)) return false;

  std::string newCommand = "ssd.exe W " + lbaStr + " " + valueStr;
  system(newCommand.c_str());

  return true;
}

bool ShellTest::excuteRead(std::istringstream &iss) {
  std::string lbaStr, trashStr;
  iss >> lbaStr >> trashStr;

  if (!checkValidArgument(trashStr)) return false;
  if (!checkValidLba(lbaStr)) return false;

  std::string newCommand = "ssd.exe R " + lbaStr;
  system(newCommand.c_str());

  return true;
}

std::string ShellTest::getOutput() { return readFromFile(output_file_name); }

std::string ShellTest::readFromFile(const std::string &output_file_name) {
  std::ifstream file(output_file_name);

  if (!file.is_open()) {
    throw std::ios_base::failure("Failed to open file: " + output_file_name);
  }

  std::string line = "";
  std::getline(file, line);
  return line;
}

bool ShellTest::checkValidArgument(std::string &trashStr) {
  if (trashStr.length() > 0) return false;
  return true;
}

bool ShellTest::checkValidLba(std::string &lbaStr) {
  if (lbaStr.length() < 0 || lbaStr.length() >= 3) return false;
  for (char lbaChar : lbaStr)
    if (lbaChar < '0' || lbaChar > '9') return false;
  return true;
}

bool ShellTest::checkValidValue(std::string &valueStr) {
  if (valueStr.length() != 10) return false;
  if (valueStr[0] != '0' || valueStr[1] != 'X') return false;
  std::string valueStrSub = valueStr.substr(2);
  for (char valueChar : valueStrSub)
    if ((valueChar < '0' || valueChar > '9') &&
        (valueChar < 'A' || valueChar > 'F'))
      return false;
  return true;
}
