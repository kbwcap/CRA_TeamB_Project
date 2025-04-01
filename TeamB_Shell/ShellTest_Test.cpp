#include "ShellTest.h"

#include "gmock/gmock.h"

TEST(CommandTest, commandValid) {
  std::stringstream buffer;
  std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

  ShellTest shellTest;
  shellTest.runShellTest("write 30 0xAAAABBBB\n");

  std::cout.rdbuf(oldCout);
  EXPECT_EQ(buffer.str(), "");
}

TEST(CommandTest, commandInvalid) {
  std::stringstream buffer;
  std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

  ShellTest shellTest;
  shellTest.runShellTest("gogo\n");

  std::cout.rdbuf(oldCout);
  EXPECT_EQ(buffer.str(), "INVALID COMMAND\n");
}

TEST(CommandTest, commandInvalid2) {
  std::stringstream buffer;
  std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

  ShellTest shellTest;
  shellTest.runShellTest("write 30 0xFFFFFFFF gogo\n");

  std::cout.rdbuf(oldCout);
  EXPECT_EQ(buffer.str(), "INVALID COMMAND\n");
}

TEST(CommandTest, LbaInvalid) {
  std::stringstream buffer;
  std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

  ShellTest shellTest;
  shellTest.runShellTest("write a3 0xAAAABBBB\n");

  std::cout.rdbuf(oldCout);
  EXPECT_EQ(buffer.str(), "INVALID COMMAND\n");
}

TEST(CommandTest, ValueInvalid) {
  std::stringstream buffer;
  std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

  ShellTest shellTest;
  shellTest.runShellTest("write 30 0xFFFFF\n");

  std::cout.rdbuf(oldCout);
  EXPECT_EQ(buffer.str(), "INVALID COMMAND\n");
}
