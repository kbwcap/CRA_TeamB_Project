#pragma once
#include "UserCommandQueue.h"
#include "ShellTest.h"

bool Test_FullWriteAndReadCompare_1() {
  ShellTest realShell;
  UserCommandQueue queue(realShell);

  const int minLba = 0;
  const int maxLba = 99;
  const unsigned int groupSize = 5;

  uint32_t seed = static_cast<uint32_t>(time(0));
  for (int base = minLba; base <= maxLba; base += groupSize + 1) {
    unsigned int pattern = patternGenerator(seed);

    for (unsigned int offset = 0; offset < groupSize && base + offset <= maxLba;
         ++offset) {
      if (!queue.enqueueWrite(base + offset, pattern)) return false;
    }
    for (unsigned int offset = 0; offset < groupSize && base + offset <= maxLba;
         ++offset) {
      if (!queue.enqueueRead(base + offset, pattern)) return false;
    }

    if (!queue.flush()) return false;
  }
  return true;
}
