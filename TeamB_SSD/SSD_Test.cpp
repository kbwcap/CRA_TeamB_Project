#include "VirtualSSD.h"
#include "gmock/gmock.h"
#include "iostream"
#include "EraseCommand.h"
#include "WriteCommand.h"
#include "FlushCommand.h"
#include "ReadCommand.h"

using namespace testing;

class CommandFixture : public Test {
 protected:
  VirtualSSD ssd;

  void SetUp() override {
    ssd.clearCommandBuff();
  }

  std::shared_ptr<ICommand> makeCommand(char cmd, int lba, uint32_t value) {
    std::shared_ptr<ICommand> ret;
    if (cmd == 'W') {
      ret = std::make_shared<WriteCommand>(ssd, lba, value);
    } else if (cmd == 'R') {
      ret = std::make_shared<ReadCommand>(ssd, lba);
    } else if (cmd == 'F') {
      ret = std::make_shared<FlushCommand>(ssd);
    } else if (cmd == 'E') {
      ret = std::make_shared<EraseCommand>(ssd, lba, value);
    }
    return ret;
  }

  void executeAndExpectTRUE(char cmd, int lba = 0, uint32_t value = 0) {
    std::shared_ptr<ICommand> Command = makeCommand(cmd, lba, value); 
    bool ret = ssd.executeCommand(Command);
    EXPECT_TRUE(ret);
  }

  void expectCommandFALSE(char cmd, int lba, uint32_t value) {
    std::shared_ptr<ICommand> Command = makeCommand(cmd, lba, value);
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

TEST_F(CommandFixture, basic_SSD_test_Read_3_TRUE) {
  executeAndExpectTRUE('R', 3, 0xAAAABBBB);
}

TEST_F(CommandFixture, basic_SSD_test_Read_3_OutOfRange) {
  expectCommandFALSE('R', 103, DEFAULT_VALUE);
}

TEST_F(CommandFixture, basic_SSD_test_Write_Wrong_lba_index) {
  expectCommandFALSE('W', 102, 0xAAAABBBB);
}

TEST_F(CommandFixture, basic_SSD_test_Flush) { executeAndExpectTRUE('F'); }

TEST_F(CommandFixture, basic_SSD_test_Erase_0_10) {
  executeAndExpectTRUE('E', 0, 10);
}

TEST_F(CommandFixture, basic_SSD_test_Erase_95_10) {
  expectCommandFALSE('E', 95, 10);
}

TEST_F(CommandFixture, basic_SSD_test_Erase_100_10) {
  expectCommandFALSE('E', 100, 10);
}

TEST_F(CommandFixture, basic_SSD_test_Erase_0_20) {
  executeAndExpectTRUE('E', 0, 20);
}

 TEST_F(CommandFixture, basic_SSD_testR_Read_Buffer_102) {
  executeAndExpectTRUE('R', 3, 0xAAAABBBB);
 }

TEST_F(CommandFixture, basic_SSD_test_Write_ignore_1) {
  executeAndExpectTRUE('W', 97, 0x1234ABCE);
  executeAndExpectTRUE('W', 17, 0x1234ABCE);
  executeAndExpectTRUE('W', 17, 0x1234ABCa);
  executeAndExpectTRUE('W', 97, 0x1234A111);
}