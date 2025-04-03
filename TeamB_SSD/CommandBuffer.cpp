#include "CommandBuffer.h"
#include "VirtualSSD.h"

CommandBuffer::CommandBuffer(VirtualSSD& ssd, const std::string& bufferFile)
  : ssd(ssd), commandCount(0), bufferFile(bufferFile) {}

CommandBuffer::~CommandBuffer() {
  saveCommandToFile();
}

void CommandBuffer::addCommand(std::shared_ptr<ICommand> command) {
  if (commandCount >= MAX_COMMANDS) {
    excuteCommand();
    clear();
  }
  commandBuffer[commandCount++] = command;
}

void CommandBuffer::reloadFromCommandFile() {
  std::ifstream file(bufferFile);

  if (!file.is_open()) {
    return;
  }

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string commandType;
    int lba;
    uint32_t dataOrSize;

    if (iss >> commandType >> lba >> dataOrSize) {
        std::shared_ptr<ICommand> command = nullptr;
      if (commandType == "W") {
        command = std::make_shared<WriteCommand>(ssd, lba, dataOrSize);
        addCommand(command);
        std::cout << "Loaded WriteCommand: W " << lba << " 0x" << std::hex << dataOrSize << std::endl;
      }
      else if (commandType == "E") {
        command = std::make_shared<EraseCommand>(ssd, lba, dataOrSize);
        addCommand(command);
        std::cout << "Loaded EraseCommand: E " << lba << " " << dataOrSize << std::endl;
      }
    }
  }
  file.close();
  clearCommandFile();
  excuteCommand();
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

void CommandBuffer::clear() {
  commandCount = 0;
  for (int i = 0; i < MAX_COMMANDS; ++i) {
    commandBuffer[i] = nullptr;
  }
  clearCommandFile();
}

void CommandBuffer::clearCommandFile() {
  std::ofstream file(bufferFile, std::ios::trunc);
  if (file.is_open()) {
    file.close();
  }
  else {
    std::cerr << "Error: Unable to open file " << bufferFile << " for clearing." << std::endl;
  }
}

void CommandBuffer::excuteCommand() {
    for (int i = 0; i < commandCount; ++i) {
        commandBuffer[i]->execute();
    }
}