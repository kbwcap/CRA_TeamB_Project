#pragma once
#include "UserCommandQueue.h"
#include "ShellTest.h"

bool Test_EraseAndWriteAging_4() {
  ShellTest realShell;
  UserCommandQueue queue(realShell);

  const int maxLoopCount = 30;
  const int len = 2;
  const int maxLba = 99;
  unsigned int pat;

  uint32_t seed = static_cast<uint32_t>(time(0));
  for (int loop = 0; loop < maxLoopCount; loop++) {
    if (!queue.enqueueEraseRange(0, len)) return false;
    for (int startLba = len; startLba + len <= maxLba; startLba += len) {
      pat = patternGenerator(seed);
      if (!queue.enqueueWrite(startLba, pat)) return false;

      pat = patternGenerator(seed);
      if (!queue.enqueueWrite(startLba, pat)) return false;
      if (!queue.enqueueEraseRange(startLba, startLba + len)) return false;
    }
  }

  return queue.flush();
}
