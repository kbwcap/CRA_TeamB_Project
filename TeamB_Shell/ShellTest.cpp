#include "ShellTest.h"

void ShellTest::executeCommand(const std::string &input) {
  std::istringstream iss(input);
  std::string command;
  iss >> command;

  if (command == "write") {
    if (!excuteWrite(iss)) std::cout << invalid_command;
    return;
  } else if (command == "read") {
    if (!excuteRead(iss)) std::cout << invalid_command;
    return;
  } else if (command == "exit") {
    exit(0);
  } else if (command == "help") {
    printHelp();
    return;
  } else if (command == "fullwrite") {
    if (!excuteFullWrite(iss)) std::cout << invalid_command;
    return;
  } else if (command == "fullread") {
    if (!excuteFullRead(iss)) std::cout << invalid_command;
    return;
  } else if (command == "erase") {
    if (!excuteErase(iss)) std::cout << invalid_command;
    return;
  } else if (command == "flush") {
    if (!excuteFlush(iss)) std::cout << invalid_command;
    return;
  } else {
    int status = testManager->runTest(input);
    if (status == testManager->NO_TC) {
      std::cout << invalid_command;
    } else {
      std::cout << "status " << status << "\n";
    }
    return;
  }
}

void ShellTest::printHelp() {
  std::cout
      << R"(Best Reviewers (A community of individuals who aspire to be the best reviewers) 
- byeongun.ko [Team Leader]
- yuz010.kim
- yunje.kim	
- hjy.park

[Commands]
- write LBA Value : Writes the specified Value to the given LBA.
- read LBA : Reads the Value from the given LBA.
- exit : Exits the Shell Test.
- fullwrite Value : Writes the specified Value to all LBAs.
- fullread : Reads values from all LBAs.
)" << std::endl;
}

bool ShellTest::excuteWrite(std::istringstream &iss) {
  std::string lbaStr, valueStr, trashStr;
  iss >> lbaStr >> valueStr >> trashStr;

  if (!checkValidArgument(trashStr)) return false;
  if (!checkValidLba(lbaStr)) return false;
  if (!checkValidValue(valueStr)) return false;

  std::string newCommand = "ssd.exe W " + lbaStr + " " + valueStr;
  system(newCommand.c_str());
  std::cout << write_done;

  return true;
}

bool ShellTest::excuteRead(std::istringstream &iss) {
  std::string lbaStr, trashStr;
  iss >> lbaStr >> trashStr;

  if (!checkValidArgument(trashStr)) return false;
  if (!checkValidLba(lbaStr)) return false;

  std::string newCommand = "ssd.exe R " + lbaStr;
  system(newCommand.c_str());
  std::cout << read_done + getOutput() + "\n";

  return true;
}

bool ShellTest::excuteFullWrite(std::istringstream &iss) {
  std::string valueStr, trashStr;
  iss >> valueStr >> trashStr;

  if (!checkValidArgument(trashStr)) return false;
  if (!checkValidValue(valueStr)) return false;

  for (int LbaNum = 0; LbaNum < MAX_LBA; LbaNum++) {
    std::string newCommand =
        "ssd.exe W " + std::to_string(LbaNum) + " " + valueStr;
    system(newCommand.c_str());
  }
  std::cout << fullwrite_done;

  return true;
}

bool ShellTest::excuteFullRead(std::istringstream &iss) {
  std::string trashStr;
  iss >> trashStr;

  if (!checkValidArgument(trashStr)) return false;

  for (int LbaNum = 0; LbaNum < MAX_LBA; LbaNum++) {
    std::string newCommand = "ssd.exe R " + std::to_string(LbaNum);
    system(newCommand.c_str());
    std::cout << read_done + getOutput() + "\n";
  }

  return true;
}

bool ShellTest::excuteErase(std::istringstream &iss) {
  std::string lbaStr, sizeStr, trashStr;
  iss >> lbaStr >> sizeStr >> trashStr;

  if (!checkValidArgument(trashStr)) return false;
  if (!checkValidLba(lbaStr)) return false;
  if (!checkValidSize(sizeStr)) {
    return false;
  }

  long long sizeNum = std::stoll(sizeStr);
  int lbaNum = std::stoi(lbaStr);
  while (sizeNum > 0) {
    int size = MAX_SIZE;
    if (size > sizeNum) size = sizeNum;

    std::string newCommand =
        "ssd.exe E " + std::to_string(lbaNum) + " " + std::to_string(size);
    system(newCommand.c_str());

    std::cout << newCommand << std::endl;

    lbaNum += size;
    sizeNum -= size;
  }

  return true;
}

bool ShellTest::excuteFlush(std::istringstream &iss) {
  std::string trashStr;
  iss >> trashStr;

  if (!checkValidArgument(trashStr)) return false;

  std::string newCommand = "ssd.exe F";
  system(newCommand.c_str());
  std::cout << flush_done;

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
  if (valueStr[0] != '0' || valueStr[1] != 'x') return false;
  std::string valueStrSub = valueStr.substr(2);
  for (char valueChar : valueStrSub)
    if ((valueChar < '0' || valueChar > '9') &&
        (valueChar < 'A' || valueChar > 'F') &&
        (valueChar < 'a' || valueChar > 'f'))
      return false;
  return true;
}

bool ShellTest::checkValidSize(std::string &sizeStr) {
  // long long 이상인 경우
  if (sizeStr.length() >= 19) {
    std::cout << err_too_big_size;
    return false;
  }
  if (sizeStr[0] < '1' || sizeStr[0] > '9') return false;

  for (char valueChar : sizeStr)
    if (valueChar < '0' || valueChar > '9') return false;

  return true;
}