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

  CommandBuffer(VirtualSSD& ssd, const std::string& bufferFile = "command_buffer.txt");

  ~CommandBuffer();

  void addCommand(std::shared_ptr<ICommand> command);

  void clear();

private:
  void reloadFromCommandFile();

  void saveCommandToFile();

  void clearCommandFile();

  std::shared_ptr<ICommand> commandBuffer[MAX_COMMANDS] = {};
  int commandCount;
  const std::string bufferFile;
  VirtualSSD& ssd;
};
