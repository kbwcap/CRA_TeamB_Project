#include "ShellTest.h"
#include "TestManager.h"
#include "gmock/gmock.h"
int main() {
#ifdef _DEBUG
  testing::InitGoogleTest();
  return RUN_ALL_TESTS();
#endif

  TestManager testManager;
  testManager.registerTest("1_FullWriteAndReadCompare",
                           +Test_FullWriteAndReadCompare_1);
  testManager.registerTest("2_PartialLBAWrite", Test_PartialLBAWrite_2);
  testManager.registerTest("3_WriteReadAging", Test_WriteReadAging_3);
  ShellTest shellTest{&testManager};
  while (true) {
    std::string input;
    std::cout << "> ";
    std::getline(std::cin, input);
    shellTest.executeCommand(input);
  }
}