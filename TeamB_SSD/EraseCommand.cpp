#include "EraseCommand.h"
#include "VirtualSSD.h"

EraseCommand::EraseCommand(VirtualSSD& ssd, int lba, int size)
  : ICommand(ssd), lba(lba), size(size) {
}

bool EraseCommand::execute() {
  if (ssd.isOutOfRange(lba)) {
    ssd.saveOutputToFile(MSG_ERROR);
    return false;
  }

  if (size <= 0) {
    ssd.saveOutputToFile(MSG_ERROR_SIZE);
    return false;
  }

  if (size > 10) {
    ssd.saveOutputToFile(MSG_ERROR_SIZE);
    size = 10;
  }

  for (int startLBA = lba; startLBA < lba + size; ++startLBA) {
    if (ssd.isOutOfRange(startLBA)) {
      ssd.saveOutputToFile(MSG_ERROR_OUT_OF_RANGE);
      return false;
    }
    ssd.setData(startLBA, 0);
  }
  return true;
}

int EraseCommand::getLBA() const {
  return lba;
}

uint32_t EraseCommand::getSize() const {
  return size;
}