#pragma once

#include "ICommand.h"
#include "WriteCommand.h"
#include "EraseCommand.h"
#include <fstream>
#include <sstream>
#include <memory>
#include <iostream>
#include "define.h"

class CommandBuffer {
public:
  CommandBuffer(VirtualSSD& ssd,
    const std::string& bufferFile = "command_buffer.txt",
    const std::string& bufferFolder = "buffer");

  ~CommandBuffer();

  void addCommand(std::shared_ptr<ICommand> command);
  void executeCommand();
  void clear();
  void reloadFromCommandFile();
  void reloadFromBufferFolderCommandFile();
  void updateBufferFile(int index, const std::string& commandType, int lba, uint32_t dataOrSize);

  bool getReadCommandBuffer(int lba, uint32_t& data);
  int getBufferSize() const;

 private:
  void renameOrCreateFile(const std::string& oldFileName, const std::string& newFileName);
  void createFile(const std::string& newFileName);
  void initializeBufferFolder();
  void saveCommandToFile();
  void clearCommandFile();

  std::shared_ptr<ICommand> setIgnoreMergeCommmand(
      std::shared_ptr<ICommand> command);
  void updateCommandBuffer();
  void removeAtBuffer(int i);
  bool isEraseInRange(int innerId, int innerSize, int outerId, int outerSize);
  bool canMerge(int id1, int size1, int id2, int size2);
  
  std::shared_ptr<ICommand> commandBuffer[MAX_COMMANDS] = {};
  int commandCount;
  const std::string bufferFile;
  const std::string bufferFolder;
  VirtualSSD& ssd;
};
