#include "ShellTest.h"
#include "TestManager.h"
#include "gmock/gmock.h"
int main() {
#ifdef _DEBUG
  testing::InitGoogleTest();
  return RUN_ALL_TESTS();
#endif

  ShellTest shellTest{};
  while (true) {
    std::string input;
    std::cout << "> ";
    std::getline(std::cin, input);
    shellTest.executeCommand(input);
  }
}