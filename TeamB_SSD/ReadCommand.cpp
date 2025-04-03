#include "ReadCommand.h"
#include "VirtualSSD.h"

bool ReadCommand::execute() {
  if (ssd.isOutOfRange(lba)) return false;

  std::stringstream ss;
  ss << "LBA " << lba << " 0x" << std::setfill('0') << std::setw(8)
    << std::hex << std::uppercase << ssd.getData(lba);

  return ssd.saveOutputToFile(ss.str());
}
