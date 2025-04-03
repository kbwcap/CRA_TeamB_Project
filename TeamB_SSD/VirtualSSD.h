#pragma once

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include "define.h"
#include "ICommand.h"
#include "CommandBuffer.h"

class VirtualSSD {
public:
  VirtualSSD(const std::string& nand = "ssd_nand.txt", const std::string& out = "ssd_output.txt");

  bool executeCommand(std::shared_ptr<ICommand> command);
  void setData(int lba, uint32_t data);
  uint32_t getData(int lba) const;

  bool saveStorageToFile();
  void loadStorageFromFile();

  bool saveOutputToFile(std::string outData);
  bool isOutOfRange(int lba);

private:
  uint32_t storage[MAX_RANGE_NUM];
  const std::string nand_file;
  const std::string out_file;

  CommandBuffer commandBuffer;
};
