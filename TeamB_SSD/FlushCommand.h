#pragma once

#include "ICommand.h"

class FlushCommand : public ICommand {
public:
  FlushCommand(VirtualSSD& ssd);
  bool execute() override;
};
