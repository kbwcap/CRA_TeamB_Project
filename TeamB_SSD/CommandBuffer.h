#pragma once

#include "ICommand.h"
#include "WriteCommand.h"
#include "EraseCommand.h"
#include <fstream>
#include <sstream>
#include <memory>
#include <iostream>

class CommandBuffer {
public:
  static constexpr int MAX_COMMANDS = 5;

  CommandBuffer(VirtualSSD& ssd,
    const std::string& bufferFile = "command_buffer.txt",
    const std::string& bufferFolder = "buffer");

  ~CommandBuffer();

  void addCommand(std::shared_ptr<ICommand> command);
  void clear();
  void reloadFromCommandFile();
  void reloadFromCommandFile2();
  void updateBufferFile(int index, const std::string& commandType, int lba, uint32_t dataOrSize);

private:
  void renameOrCreateFile(const std::string& oldFileName, const std::string& newFileName);
  void createFile(const std::string& newFileName);
  void initializeBufferFolder();
  void saveCommandToFile();
  void clearCommandFile();
  void executeCommand();

  std::shared_ptr<ICommand> commandBuffer[MAX_COMMANDS] = {};
  int commandCount;
  const std::string bufferFile;
  const std::string bufferFolder;
  VirtualSSD& ssd;
};
