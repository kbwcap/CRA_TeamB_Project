#pragma once
#include "TestManager.h"

extern bool Test_FullWriteAndReadCompare_1();
extern bool Test_PartialLBAWrite_2();
extern bool Test_WriteReadAging_3();
extern bool Test_EraseAndWriteAging_4();
extern bool Test_FullWriteFullReadFlush_5();
extern bool Test_FullWriteEraseAllFlushFullRead_6();

extern "C" __declspec(dllexport) void registerTests(TestManager* tm) {
  tm->registerTest("1_FullWriteAndReadCompare", Test_FullWriteAndReadCompare_1);
  tm->registerTest("2_PartialLBAWrite", Test_PartialLBAWrite_2);
  tm->registerTest("3_WriteReadAging", Test_WriteReadAging_3);
  tm->registerTest("4_EraseAndWriteAging", Test_EraseAndWriteAging_4);
  tm->registerTest("5_FullWriteFullReadFlush", Test_FullWriteFullReadFlush_5);
  tm->registerTest("6_FullWriteEraseAllFlushFullRead",
                   Test_FullWriteEraseAllFlushFullRead_6);
}