#pragma once
#include "../RegisterHelper.h"
#include "../UserCommandQueue.h"

bool Test_FullWriteFullReadFlush_5() {
  ShellTest realShell;
  UserCommandQueue queue(realShell);
  unsigned int pat;

  uint32_t seed = static_cast<uint32_t>(time(0));
  pat = patternGenerator(seed);
  if (!queue.enqueueFullWrite(pat)) return false;
  if (!queue.enqueueFullRead()) return false;
  if (!queue.enqueueFlush()) return false;

  return queue.flush();
}

REGISTER_TEST("5_FullWriteFullReadFlush", Test_FullWriteFullReadFlush_5);