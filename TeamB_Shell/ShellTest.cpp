#include "ShellTest.h"

#include "logger.h"

Logger logger;

void ShellTest::executeCommand(const std::string &input) {
  std::istringstream iss(input);
  std::string command;
  iss >> command;

  bool isValidCommand = true;

  if (command == "write")
    isValidCommand = executeWrite(iss);
  else if (command == "read")
    isValidCommand = executeRead(iss);
  else if (command == "exit")
    exit(0);
  else if (command == "help")
    printHelp();
  else if (command == "fullwrite")
    isValidCommand = executeFullWrite(iss);
  else if (command == "fullread")
    isValidCommand = executeFullRead(iss);
  else if (command == "erase")
    isValidCommand = executeErase(iss);
  else if (command == "erase_range")
    isValidCommand = executeEraseRange(iss);
  else if (command == "flush")
    isValidCommand = executeFlush(iss);
  else if (command == script_file_name)
    runner();
  else {
    int status = testManager->runTest(input);
    if (status == testManager->NO_TC)
      isValidCommand = false;
    else if (status == testManager->PASS)
      std::cout << "PASS" << std::endl;
    else if (status == testManager->FAIL)
      std::cout << "FAIL" << std::endl;
  }

  if (!isValidCommand) std::cout << invalid_command;
}

bool ShellTest::executeWrite(std::istringstream &iss) {
  logger.print(func_execute_write, status_start);
  std::string lbaStr, valueStr, trashStr;
  iss >> lbaStr >> valueStr >> trashStr;

  if (!checkValidArgument(trashStr)) return false;
  if (!checkValidLba(lbaStr)) return false;
  if (!checkValidValue(valueStr)) return false;

  std::string sendCommand = "ssd.exe W " + lbaStr + " " + valueStr;
  system(sendCommand.c_str());
  std::cout << write_done;

  logger.print(func_execute_write, status_finish);
  return true;
}

bool ShellTest::executeRead(std::istringstream &iss) {
  logger.print(func_execute_read, status_start);
  std::string lbaStr, trashStr;
  iss >> lbaStr >> trashStr;

  if (!checkValidArgument(trashStr)) return false;
  if (!checkValidLba(lbaStr)) return false;

  std::string sendCommand = "ssd.exe R " + lbaStr;
  system(sendCommand.c_str());
  std::cout << read_done + getOutput() + "\n";

  logger.print(func_execute_read, status_finish);
  return true;
}

void ShellTest::printHelp() { std::cout << help_command << std::endl; }

bool ShellTest::executeFullWrite(std::istringstream &iss) {
  logger.print(func_execute_fullwrite, status_start);
  std::string valueStr, trashStr;
  iss >> valueStr >> trashStr;

  if (!checkValidArgument(trashStr)) return false;
  if (!checkValidValue(valueStr)) return false;

  for (int LbaNum = 0; LbaNum < MAX_LBA; LbaNum++) {
    std::string sendCommand =
        "ssd.exe W " + std::to_string(LbaNum) + " " + valueStr;
    system(sendCommand.c_str());
  }
  std::cout << fullwrite_done;

  logger.print(func_execute_fullwrite, status_finish);
  return true;
}

bool ShellTest::executeFullRead(std::istringstream &iss) {
  logger.print(func_execute_fullread, status_start);
  std::string trashStr;
  iss >> trashStr;

  if (!checkValidArgument(trashStr)) return false;

  for (int LbaNum = 0; LbaNum < MAX_LBA; LbaNum++) {
    std::string sendCommand = "ssd.exe R " + std::to_string(LbaNum);
    system(sendCommand.c_str());
    std::cout << read_done + getOutput() + "\n";
  }

  logger.print(func_execute_fullread, status_finish);
  return true;
}

bool ShellTest::executeErase(std::istringstream &iss) {
  logger.print(func_execute_erase, status_start);
  std::string lbaStr, sizeStr, trashStr;
  iss >> lbaStr >> sizeStr >> trashStr;

  if (!checkValidArgument(trashStr)) return false;
  if (!checkValidLba(lbaStr)) return false;
  if (!checkValidSize(sizeStr)) return false;

  sendEraseCommand(std::stoll(sizeStr), std::stoi(lbaStr));
  logger.print(func_execute_erase, status_finish);
  return true;
}

bool ShellTest::executeEraseRange(std::istringstream &iss) {
  logger.print(func_execute_erase_range, status_start);
  std::string startLbaStr, endLbaStr, trashStr;
  iss >> startLbaStr >> endLbaStr >> trashStr;

  if (!checkValidArgument(trashStr)) return false;
  if (!checkValidLba(startLbaStr)) return false;
  if (!checkValidLba(endLbaStr)) return false;

  int startLbaNum = std::stoi(startLbaStr);
  int endLbaNum = std::stoi(endLbaStr);

  if (startLbaNum > endLbaNum) {
    logger.print(func_execute_erase_range,
                 "[Invalid] Start LBA bigger than End LBA.");
    std::cout << err_start_bigger_than_end;
    return false;
  }

  sendEraseCommand(endLbaNum - startLbaNum + 1, startLbaNum);
  logger.print(func_execute_erase_range, status_finish);
  return true;
}

bool ShellTest::executeFlush(std::istringstream &iss) {
  logger.print(func_execute_flush, status_start);
  std::string trashStr;
  iss >> trashStr;

  if (!checkValidArgument(trashStr)) return false;

  std::string sendCommand = "ssd.exe F";
  system(sendCommand.c_str());
  std::cout << flush_done;

  logger.print(func_execute_flush, status_finish);
  return true;
}

void ShellTest::runner() {
  std::ifstream file(script_file_name);
  if (!file.is_open()) {
    throw std::ios_base::failure("Failed to open file: " + script_file_name);
  }

  std::vector<std::string> testScripts;
  std::string testcase;
  while (std::getline(file, testcase)) {
    std::cout << testcase;
    testScripts.push_back(testcase);
  }
  file.close();

  int status;
  for (const auto &testcase : testScripts) {
    status = testManager->runTest(testcase);
    std::cout << testcase << "  ----  Run...";
    if (status == testManager->FAIL) {
      std::cout << "FAIL!\n";
      return;
    } else if (status == testManager->NO_TC) {
      return;
    }
    std::cout << "PASS\n";
  }
  return;
}

std::string ShellTest::getOutput() { return readFromFile(output_file_name); }

std::string ShellTest::readFromFile(const std::string &output_file_name) {
  std::ifstream file(output_file_name);

  if (!file.is_open()) {
    logger.print("readFromFile()", "[Error] output file can not open.");
    throw std::ios_base::failure("Failed to open file: " + output_file_name);
  }

  std::string line = "";
  std::getline(file, line);
  return line;
}

void ShellTest::sendEraseCommand(long long sizeNum, int lbaNum) {
  while (sizeNum > 0) {
    int size = MAX_SIZE;
    if (size > sizeNum) size = sizeNum;

    std::string sendCommand =
        "ssd.exe E " + std::to_string(lbaNum) + " " + std::to_string(size);
    system(sendCommand.c_str());

    std::cout << erase_done << sendCommand << std::endl;

    lbaNum += size;
    sizeNum -= size;
  }
}

bool ShellTest::checkValidArgument(std::string &trashStr) {
  if (trashStr.length() > 0) {
    logger.print("checkValidArgument()", "[Invalid] Too many arguments.");
    return false;
  }
  return true;
}

bool ShellTest::checkValidLba(std::string &lbaStr) {
  if (lbaStr.length() < 0 || lbaStr.length() >= 3) {
    logger.print("checkValidLba()", "[Invalid] LBA is invalid.");
    return false;
  }
  for (char lbaChar : lbaStr)
    if (lbaChar < '0' || lbaChar > '9') {
      logger.print("checkValidLba()", "[Invalid] LBA is invalid.");
      return false;
    }
  return true;
}

bool ShellTest::checkValidValue(std::string &valueStr) {
  if (valueStr.length() != 10) {
    logger.print("checkValidValue()", "[Invalid] Value must be 10 in length.");
    return false;
  }
  if (valueStr[0] != '0' || valueStr[1] != 'x') {
    logger.print("checkValidValue()", "[Invalid] Value must be start with 0x.");
    return false;
  }
  // 0x를 제외한 8자리의 유효성 검사
  std::string valueStrSub = valueStr.substr(2);
  for (char valueChar : valueStrSub)
    if ((valueChar < '0' || valueChar > '9') &&
        (valueChar < 'A' || valueChar > 'F') &&
        (valueChar < 'a' || valueChar > 'f')) {
      logger.print("checkValidValue()", "[Invalid] Value is invalid.");
      return false;
    }
  return true;
}

bool ShellTest::checkValidSize(std::string &sizeStr) {
  // long long 이상인 경우
  if (sizeStr.length() >= 19) {
    std::cout << err_too_big_size;
    logger.print("checkValidSize()", "[Invalid] Size is too big.");
    return false;
  }
  if (sizeStr[0] < '1' || sizeStr[0] > '9') {
    logger.print("checkValidSize()",
                 "[Invalid] Size must be a number that does not start with 0.");
    return false;
  }

  for (char valueChar : sizeStr)
    if (valueChar < '0' || valueChar > '9') {
      logger.print("checkValidSize()", "[Invalid] Size must be a number.");
      return false;
    }
  return true;
}