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

TEST_F(CommandFixture, commandValid) {
  shellTest.runShellTest("write 30 0xAAAABBBB\n");
  EXPECT_EQ(buffer.str(), "");
}

TEST_F(CommandFixture, commandInvalid) {
  shellTest.runShellTest("gogo\n");
  EXPECT_EQ(buffer.str(), "INVALID COMMAND\n");
}

TEST_F(CommandFixture, commandInvalid2) {
  shellTest.runShellTest("write 30 0xFFFFFFFF gogo\n");
  EXPECT_EQ(buffer.str(), "INVALID COMMAND\n");
}

TEST_F(CommandFixture, LbaInvalid) {
  shellTest.runShellTest("write a3 0xAAAABBBB\n");
  EXPECT_EQ(buffer.str(), "INVALID COMMAND\n");
}

TEST_F(CommandFixture, ValueInvalid) {
  shellTest.runShellTest("write 30 0xFFFFF\n");
  EXPECT_EQ(buffer.str(), "INVALID COMMAND\n");
}
