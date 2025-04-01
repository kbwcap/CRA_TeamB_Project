#include "ShellTest.h"
#include "gmock/gmock.h"

int main() {
#ifdef UT
  testing::InitGoogleTest();
  return RUN_ALL_TESTS();
#endif

  while (true) {
    ShellTest shellTest;
    std::string input;
    std::cout << "> ";
    std::getline(std::cin, input);
    shellTest.runShellTest(input);
  }
}