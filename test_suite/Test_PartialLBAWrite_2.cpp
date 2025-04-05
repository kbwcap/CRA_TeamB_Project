#pragma once
#include "UserCommandQueue.h"
#include "ShellTest.h"

bool Test_PartialLBAWrite_2() {
  ShellTest realShell;
  UserCommandQueue queue(realShell);

  const std::vector<int> lbaOrder = {4, 0, 3, 1, 2};
  const int loopCount = 30;
  uint32_t seed = static_cast<uint32_t>(time(0));
  for (int loop = 0; loop < loopCount; ++loop) {
    unsigned int pattern = patternGenerator(seed);

    // Write sequence
    for (int lba : lbaOrder) {
      if (!queue.enqueueWrite(lba, pattern)) return false;
    }

    // ReadCompare sequence
    for (int lba : lbaOrder) {
      if (!queue.enqueueRead(lba, pattern)) return false;
    }
  }

  return queue.flush();
}
