#include "CommandBuffer.h"

CommandBuffer::CommandBuffer(VirtualSSD& ssd, const std::string& bufferFile)
  : ssd(ssd), commandCount(0), bufferFile(bufferFile) {
  reloadFromCommandFile();
}

CommandBuffer::~CommandBuffer() {
  saveCommandToFile();
}

void CommandBuffer::addCommand(std::shared_ptr<ICommand> command) {
  if (commandCount >= MAX_COMMANDS) {
    flush();
  }
  commandBuffer[commandCount++] = command;
}

void CommandBuffer::flush() {
  std::ofstream file(bufferFile, std::ios::trunc);
  if (file.is_open()) {
    for (int i = 0; i < commandCount; ++i) {
      std::stringstream ss;
      if (auto writeCommand = dynamic_cast<WriteCommand*>(commandBuffer[i].get())) {
        ss << "W " << writeCommand->getLBA() << " " << writeCommand->getData() << std::endl;
      }
      else if (auto eraseCommand = dynamic_cast<EraseCommand*>(commandBuffer[i].get())) {
        ss << "E " << eraseCommand->getLBA() << " " << eraseCommand->getSize() << std::endl;
      }
      file << ss.str();
      commandBuffer[i]->execute();
    }
    file.close();
    std::cout << "Commands flushed to file: " << bufferFile << std::endl;
  }
  commandCount = 0;
}

void CommandBuffer::reloadFromCommandFile() {
  std::ifstream file(bufferFile);

  if (!file.is_open()) {
    std::cout << "Command buffer file not found, starting with an empty buffer." << std::endl;
    return;
  }

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string commandType;
    int lba;
    uint32_t dataOrSize;

    if (iss >> commandType >> lba >> dataOrSize) {
      if (commandType == "W") {
        auto command = std::make_shared<WriteCommand>(ssd, lba, dataOrSize);
        addCommand(command); // WriteCommand 추가
        std::cout << "Loaded WriteCommand: W " << lba << " " << dataOrSize << std::endl;
      }
      else if (commandType == "E") {
        auto command = std::make_shared<EraseCommand>(ssd, lba, dataOrSize);
        addCommand(command); // EraseCommand 추가
        std::cout << "Loaded EraseCommand: E " << lba << " " << dataOrSize << std::endl;
      }
    }
  }

  file.close();
}

void CommandBuffer::saveCommandToFile() {
  std::ofstream file(bufferFile, std::ios::trunc);
  if (file.is_open()) {
    for (int i = 0; i < commandCount; ++i) {
      std::stringstream ss;
      if (auto writeCommand = dynamic_cast<WriteCommand*>(commandBuffer[i].get())) {
        ss << "W " << writeCommand->getLBA() << " " << writeCommand->getData() << std::endl;
      }
      else if (auto eraseCommand = dynamic_cast<EraseCommand*>(commandBuffer[i].get())) {
        ss << "E " << eraseCommand->getLBA() << " " << eraseCommand->getSize() << std::endl;
      }
      file << ss.str();
    }
    file.close();
  }
}
