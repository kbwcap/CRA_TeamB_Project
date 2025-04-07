#include <fstream>
#include <sstream>
#include "ICommand.h"
#include "VirtualSSD.h"

VirtualSSD::VirtualSSD(const std::string& nand, const std::string& out)
  : nand_file(nand), out_file(out), commandBuffer(*this) {
  for (int i = 0; i < MAX_RANGE_NUM; ++i) {
    storage[i] = 0;
  }
  commandBuffer.reloadFromCommandFile();
  commandBuffer.reloadFromBufferFolderCommandFile();
  loadStorageFromFile();
}

bool VirtualSSD::executeCommand(std::shared_ptr<ICommand> command) {
  if (auto writeCommand = dynamic_cast<WriteCommand*>(command.get())) {
    commandBuffer.addCommand(command);
    return true;
  }
  else if (auto eraseCommand = dynamic_cast<EraseCommand*>(command.get())) {
    commandBuffer.addCommand(command);
    return true;
  }
  return command->execute();
}

void VirtualSSD::setData(int lba, uint32_t data) {
  storage[lba] = data;
}

uint32_t VirtualSSD::readData(int lba) {
  // buffer에 read 하려는 값이 있는 경우 buffer에서 가지고 온다.
  uint32_t data = DEFAULT_VALUE;
  if (commandBuffer.getReadCommandBuffer(lba, data)) {
    return data;
  }
  return getData(lba);
}

uint32_t VirtualSSD::getData(int lba) const {
  return storage[lba];
}

bool VirtualSSD::saveStorageToFile() {
  std::ofstream outFile(nand_file, std::ios::trunc);
  
  if (outFile.is_open()) {
    for (int lba = 0; lba < MAX_RANGE_NUM; ++lba) {
      if (storage[lba]) {
        outFile << std::dec << lba << " 0x" << std::uppercase << std::hex
          << std::setw(8) << std::setfill('0') << storage[lba]
          << std::endl;
      }
    }
    outFile.close();
  }
  else {
    std::cout << "Error opening file!" << std::endl;
    return false;
  }
  return true;
}

void VirtualSSD::loadStorageFromFile() {
  std::ifstream file(nand_file);

  if (!file.is_open()) {
    return;
  }

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);

    uint32_t index;
    std::string hexValue;

    if (iss >> std::dec >> index >> std::hex >> hexValue) {
      uint32_t value = 0;
      std::stringstream(hexValue) >> std::hex >> value;
      storage[index] = value;
    }
  }
}

bool VirtualSSD::saveOutputToFile(std::string outData) {
  std::ofstream outputFile(out_file, std::ios::out);
  outputFile << outData << '\n';
  outputFile.close();
  return true;
}

bool VirtualSSD::isOutOfRange(int lba) {
  if (lba < 0 || lba >= MAX_RANGE_NUM) {
    saveOutputToFile(MSG_ERROR);
    return true;
  }
  return false;
}