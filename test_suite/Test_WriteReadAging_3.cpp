#pragma once
#include "UserCommandQueue.h"
#include "ShellTest.h"

bool Test_WriteReadAging_3() {
  ShellTest realShell;
  UserCommandQueue queue(realShell);

  const int maxLoopCount = 200;
  unsigned int pat1, pat2;

  uint32_t seed = static_cast<uint32_t>(time(0));
  for (int loop = 0; loop < maxLoopCount; loop++) {
    pat1 = patternGenerator(seed);
    pat2 = patternGenerator(seed);
    if (!queue.enqueueWrite(0, pat1)) return false;
    if (!queue.enqueueWrite(99, pat2)) return false;

    if (!queue.enqueueRead(0, pat1)) return false;
    if (!queue.enqueueRead(99, pat2)) return false;
  }

  return queue.flush();
}
