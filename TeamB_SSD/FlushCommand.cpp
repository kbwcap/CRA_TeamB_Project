#include "FlushCommand.h"
#include "VirtualSSD.h"

FlushCommand::FlushCommand(VirtualSSD& ssd) : ICommand(ssd) {}

bool FlushCommand::execute() {
  ssd.excuteAndClearCommandBuff();
  return ssd.saveStorageToFile();
}
