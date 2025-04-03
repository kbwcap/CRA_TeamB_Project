#include "WriteCommand.h"
#include "VirtualSSD.h"

WriteCommand::WriteCommand(VirtualSSD& ssd, int lba, uint32_t data)
  : ICommand(ssd), lba(lba), data(data) {
}

bool WriteCommand::execute() {
  if (ssd.isOutOfRange(lba)) return false;
  ssd.setData(lba, data);

  return ssd.saveStorageToFile();
}

int WriteCommand::getLBA() const {
  return lba;
}

uint32_t WriteCommand::getData() const {
  return data;
}