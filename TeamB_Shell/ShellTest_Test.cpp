#include "ShellTest.h"

#include "gmock/gmock.h"

using namespace testing;

class CommandFixture : public Test {
 protected:
  CommandFixture() { oldCout = std::cout.rdbuf(buffer.rdbuf()); }
  ~CommandFixture() { std::cout.rdbuf(oldCout); }
  ShellTest shellTest;
  std::stringstream buffer;

 private:
  std::streambuf* oldCout;
};

class MockShellTest : public ShellTest {
 public:
  MOCK_METHOD(std::string, getOutput, (), (override));
};

TEST_F(CommandFixture, CmdInvalid) {
  shellTest.executeCommand("gogo\n");
  EXPECT_EQ(buffer.str(), invalid_command);
}

TEST_F(CommandFixture, WriteValid) {
  shellTest.executeCommand("write 30 0xAAAABBBB\n");
  EXPECT_EQ(buffer.str(), write_done);
}

TEST_F(CommandFixture, WriteCmdInvalid) {
  shellTest.executeCommand("write 30 0xFFFFFFFF gogo\n");
  EXPECT_EQ(buffer.str(), invalid_command);
}

TEST_F(CommandFixture, WriteLbaInvalid) {
  shellTest.executeCommand("write a3 0xAAAABBBB\n");
  EXPECT_EQ(buffer.str(), invalid_command);
}

TEST_F(CommandFixture, WriteValueInvalid) {
  shellTest.executeCommand("write 30 0xFFFFF\n");
  EXPECT_EQ(buffer.str(), invalid_command);
}

TEST_F(CommandFixture, ReadValid) {
  NiceMock<MockShellTest> mockShell;

  // ssd_output.txt 파일이 존재하지 않는 경우에도 테스트하기 위해 Mock을 사용
  EXPECT_CALL(mockShell, getOutput())
      .Times(1)
      .WillRepeatedly(Return("LBA 30 0xFFFFFFFF"));

  mockShell.executeCommand("read 30\n");
  EXPECT_EQ(buffer.str(), read_done + "LBA 30 0xFFFFFFFF\n");
}

TEST_F(CommandFixture, ReadCmdInvalid) {
  shellTest.executeCommand("read 30 gogo\n");
  EXPECT_EQ(buffer.str(), invalid_command);
}

TEST_F(CommandFixture, ReadLbaInvalid) {
  shellTest.executeCommand("read a3\n");
  EXPECT_EQ(buffer.str(), invalid_command);
}

// 존재하지 않는 파일을 읽어 exception 발생시킴
TEST_F(CommandFixture, FileNotExistException) {
  EXPECT_THROW(shellTest.readFromFile("yunje_Not_Exist_file.txt"),
               std::ios_base::failure);
}

TEST_F(CommandFixture, FullWriteValid) {
  shellTest.executeCommand("fullwrite 0xAAAABBBB\n");
  EXPECT_EQ(buffer.str(), fullwrite_done);
}

TEST_F(CommandFixture, FullWriteCmdInvalid) {
  shellTest.executeCommand("fullwrite 0xFFFFFFFF gogo\n");
  EXPECT_EQ(buffer.str(), invalid_command);
}

TEST_F(CommandFixture, FullWriteValueInvalid) {
  shellTest.executeCommand("fullwrite 0xFFFFF\n");
  EXPECT_EQ(buffer.str(), invalid_command);
}

TEST_F(CommandFixture, FullReadValid) {
  NiceMock<MockShellTest> mockShell;

  // ssd_output.txt 파일이 존재하지 않는 경우에도 테스트하기 위해 Mock을 사용
  EXPECT_CALL(mockShell, getOutput())
      .WillRepeatedly(Return("LBA 30 0xFFFFFFFF"));

  mockShell.executeCommand("fullread\n");

  std::string expected = "";
  for (int LbaNum = 0; LbaNum < MAX_LBA; LbaNum++)
    expected += read_done + "LBA 30 0xFFFFFFFF\n";
  EXPECT_EQ(buffer.str(), expected);
}

TEST_F(CommandFixture, FullReadCmdInvalid) {
  shellTest.executeCommand("fullread gogo\n");
  EXPECT_EQ(buffer.str(), invalid_command);
}

TEST_F(CommandFixture, FlushVaild) {
  shellTest.executeCommand("flush\n");
  EXPECT_EQ(buffer.str(), flush_done);
}

TEST_F(CommandFixture, FlushCmdInvaild) {
  shellTest.executeCommand("flush gogo\n");
  EXPECT_EQ(buffer.str(), invalid_command);
}

TEST_F(CommandFixture, EraseValid) {
  shellTest.executeCommand("erase 30 15\n");
  std::string expected =
      erase_done + "ssd.exe E 30 10\n" + erase_done + "ssd.exe E 40 5\n";
  EXPECT_EQ(buffer.str(), expected);
}

TEST_F(CommandFixture, EraseCmdInvalid) {
  shellTest.executeCommand("erase 30 0xFFFFFFFF gogo\n");
  EXPECT_EQ(buffer.str(), invalid_command);
}

TEST_F(CommandFixture, EraseLbaInvalid) {
  shellTest.executeCommand("erase a3 0xAAAABBBB\n");
  EXPECT_EQ(buffer.str(), invalid_command);
}

TEST_F(CommandFixture, EraseValueInvalid) {
  shellTest.executeCommand("erase 30 0xFF\n");
  EXPECT_EQ(buffer.str(), invalid_command);
}

TEST_F(CommandFixture, EraseTooBigValueInvalid) {
  shellTest.executeCommand("erase 30 10000000000000000000\n");
  EXPECT_EQ(buffer.str(), err_too_big_size + invalid_command);
}

TEST_F(CommandFixture, EraseRange_Valid) {
  shellTest.executeCommand("erase_range 21 42\n");
  std::string expected = erase_done + "ssd.exe E 21 10\n" + erase_done +
                         "ssd.exe E 31 10\n" + erase_done + "ssd.exe E 41 2\n";
  EXPECT_EQ(buffer.str(), expected);
}

TEST_F(CommandFixture, EraseRange_Valid2) {
  shellTest.executeCommand("erase_range 57 83\n");
  std::string expected = erase_done + "ssd.exe E 57 10\n" + erase_done +
                         "ssd.exe E 67 10\n" + erase_done + "ssd.exe E 77 7\n";
  EXPECT_EQ(buffer.str(), expected);
}

TEST_F(CommandFixture, EraseRange_Cmd_Invalid) {
  shellTest.executeCommand("erase_range 10 20 gogo\n");
  EXPECT_EQ(buffer.str(), invalid_command);
}

TEST_F(CommandFixture, EraseRange_LBA_Invalid) {
  shellTest.executeCommand("erase_range 30 102\n");
  EXPECT_EQ(buffer.str(), invalid_command);
}

TEST_F(CommandFixture, EraseRange_startLBA_bigger_than_endLBA_Invalid) {
  shellTest.executeCommand("erase_range 30 10\n");
  EXPECT_EQ(buffer.str(), err_start_bigger_than_end + invalid_command);
}