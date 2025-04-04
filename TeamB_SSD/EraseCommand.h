#pragma once

#include "ICommand.h"

class EraseCommand : public ICommand {
public:
  EraseCommand(VirtualSSD& ssd, int lba, int size);
  bool execute() override;
  int getLBA() const;
  uint32_t getSize() const;
private:
  int lba;
  int size;
};
