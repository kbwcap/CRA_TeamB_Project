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
  EXPECT_EQ(buffer.str(), "INVALID COMMAND\n");
}

TEST_F(CommandFixture, WriteValid) {
  shellTest.executeCommand("write 30 0xAAAABBBB\n");
  EXPECT_EQ(buffer.str(), "[Write] Done\n");
}

TEST_F(CommandFixture, WriteCmdInvalid) {
  shellTest.executeCommand("write 30 0xFFFFFFFF gogo\n");
  EXPECT_EQ(buffer.str(), "INVALID COMMAND\n");
}

TEST_F(CommandFixture, WriteLbaInvalid) {
  shellTest.executeCommand("write a3 0xAAAABBBB\n");
  EXPECT_EQ(buffer.str(), "INVALID COMMAND\n");
}

TEST_F(CommandFixture, WriteValueInvalid) {
  shellTest.executeCommand("write 30 0xFFFFF\n");
  EXPECT_EQ(buffer.str(), "INVALID COMMAND\n");
}

TEST_F(CommandFixture, ReadValid) {
  NiceMock<MockShellTest> mockShell;

  // ssd_output.txt ������ �������� �ʴ� ��쿡�� �׽�Ʈ�ϱ� ���� Mock�� ���
  EXPECT_CALL(mockShell, getOutput())
      .Times(1)
      .WillRepeatedly(Return("LBA 30 0xFFFFFFFF"));

  mockShell.executeCommand("read 30\n");
  EXPECT_EQ(buffer.str(), "[Read] LBA 30 0xFFFFFFFF\n");
}

TEST_F(CommandFixture, ReadCmdInvalid) {
  shellTest.executeCommand("read 30 gogo\n");
  EXPECT_EQ(buffer.str(), "INVALID COMMAND\n");
}

TEST_F(CommandFixture, ReadLbaInvalid) {
  shellTest.executeCommand("read a3\n");
  EXPECT_EQ(buffer.str(), "INVALID COMMAND\n");
}

// �������� �ʴ� ������ �о� exception �߻���Ŵ
TEST_F(CommandFixture, FileNotExistException) {
  EXPECT_THROW(shellTest.readFromFile("yunje_Not_Exist_file.txt"),
               std::ios_base::failure);
}