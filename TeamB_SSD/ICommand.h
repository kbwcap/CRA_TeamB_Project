#pragma once

#include <memory>
class VirtualSSD;

class ICommand {
public:
  virtual ~ICommand() = default;
  virtual bool execute() = 0;

protected:
  VirtualSSD& ssd;

  ICommand(VirtualSSD& ssd) : ssd(ssd) {}
};
