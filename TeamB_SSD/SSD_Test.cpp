#include "VirtualSSD.cpp"
#include "gmock/gmock.h"
#include "iostream"

using namespace testing;

class CommandFixture : public Test {
 protected:
  VirtualSSD ssd;

  void executeAndExpectTRUE(char cmd, int lba, uint32_t value) {
    std::shared_ptr<ICommand> Command;
  if (cmd == 'W' || cmd == 'w') {
    Command = std::make_shared<WriteCommand>(ssd, lba, value);
  }
  else if (cmd == 'R' || cmd == 'r') {
    Command = std::make_shared<ReadCommand>(ssd, lba);
  }
    bool ret = ssd.executeCommand(Command);
    EXPECT_TRUE(ret);
  }

  void expectCommandFALSE(char cmd, int lba, uint32_t value) {
    std::shared_ptr<ICommand> Command;
    if (cmd == 'W' || cmd == 'w') {
      Command = std::make_shared<WriteCommand>(ssd, lba, value);
    }
    else if (cmd == 'R' || cmd == 'r') {
      Command = std::make_shared<ReadCommand>(ssd, lba);
    }
    bool ret = ssd.executeCommand(Command);
    EXPECT_FALSE(ret);
  }
};

TEST_F(CommandFixture, basic_SSD_test_Write_1) {
  executeAndExpectTRUE('W', 3, 0xAAAABBBB);
}

TEST_F(CommandFixture, basic_SSD_test_Write_2) {
  executeAndExpectTRUE('W', 3, 0xAAAABBBB);
  executeAndExpectTRUE('W', 97, 0x1234ABCD);
}

TEST_F(CommandFixture, basic_SSD_test_Read_3_TRUE)  
{
  executeAndExpectTRUE('R', 3, 0xAAAABBBB);
}

TEST_F(CommandFixture, basic_SSD_test_Read_3_OutOfRange) {
  expectCommandFALSE('R', 103, 0x00000000);
}

 TEST_F(CommandFixture, basic_SSD_test_Write_Wrong_lba_index) {
   expectCommandFALSE('W', 102, 0xAAAABBBB);
 }
