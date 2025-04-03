#pragma once

#include "ICommand.h"

class WriteCommand : public ICommand {
public:
  WriteCommand(VirtualSSD& ssd, int lba, uint32_t data);
  bool execute() override;
  int getLBA() const;
  uint32_t getData() const;
private:
  int lba;
  uint32_t data;
};
