#pragma once

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include "define.h"
#include "ICommand.h"

class VirtualSSD {
public:
  VirtualSSD(const std::string& nand = "ssd_nand.txt", const std::string& out = "ssd_output.txt");  // 생성자 선언만

  bool executeCommand(std::shared_ptr<ICommand> command);  // 함수 선언만
  void setData(int lba, uint32_t data);  // 함수 선언만
  uint32_t getData(int lba) const;  // 함수 선언만

  bool saveStorageToFile();  // 함수 선언만
  void loadStorageFromFile();  // 함수 선언만

  bool saveOutputToFile(std::string outData);  // 함수 선언만
  bool isOutOfRange(int lba);  // 함수 선언만

private:
  uint32_t storage[MAX_RANGE_NUM];
  const std::string nand_file;
  const std::string out_file;
};
