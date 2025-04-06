#include "ShellTest.h"
#include "TestManager.h"
#include "gmock/gmock.h"
#include "define.h"
#include <Windows.h>

int main(int argc, char* argv[]) {
#ifdef _DEBUG
  testing::InitGoogleTest();
  return RUN_ALL_TESTS();
#endif

  TestManager::instance().loadTestSuitesDLL();
  ShellTest shellTest{};

  if (argc == 2 && argv[1] == script_file_name) {
    shellTest.executeCommand(script_file_name);
  }

  while (true) {
    std::string input;
    std::cout << "> ";
    std::getline(std::cin, input);
    shellTest.executeCommand(input);
  }

  return 0;
}