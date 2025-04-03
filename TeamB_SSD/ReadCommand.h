#pragma once

#include "ICommand.h"

class ReadCommand : public ICommand {
public:
  ReadCommand(VirtualSSD& ssd, int lba) : ICommand(ssd), lba(lba) {}

  bool execute() override;

private:
  int lba;
};