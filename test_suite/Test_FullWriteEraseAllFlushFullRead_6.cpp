#pragma once
#include "ShellTest.h"
#include "UserCommandQueue.h"

bool Test_FullWriteEraseAllFlushFullRead_6() {
  ShellTest realShell;
  UserCommandQueue queue(realShell);

  if (!queue.enqueueFullWrite(0xFFFFFFFF)) return false;
  if (!queue.enqueueEraseRange(0, MAX_LBA - 1)) return false;
  if (!queue.enqueueFlush()) return false;
  for (int lba = 0; lba < MAX_LBA; lba++) {
    if (!queue.enqueueRead(lba, 0)) return false;
  }

  return queue.flush();
}
