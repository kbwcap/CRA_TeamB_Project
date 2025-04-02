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
  } else {
    // Test Script 수행
    // if(!TestManager::runTest)
    std::cout << invalid_command;
  }
}

void ShellTest::printHelp() {
  std::cout
      << R"(Best Reviewers (최고의 Reviewer를 지향하는 사람들의 모임) 
- 고병운 [팀장]
- 김유증
- 김윤제
- 박화영

[명령어]
- write LBA Value : LBA에 Value를 씁니다.
- read LBA : LBA의 Value를 읽습니다.
- exit : Shell Test를 종료합니다.
- fullwrite Value : 전체 LBA에 Value를 씁니다.
- fullread : 전체 LBA를 읽습니다.
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
