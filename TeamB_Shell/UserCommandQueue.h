#pragma once
#include <string>
#include <vector>

#include "MockShell.h"
#include "TestManager.h"
#include "iostream"
using std::string;
using std::vector;
using ::testing::Sequence;

enum class CommandType { Write, Read };

struct ShellCommand {
  CommandType type;
  int lba;
  unsigned int data;           // Only for write
  std::string expectedOutput;  // Only for read
};

class UserCommandQueue {
 public:
  UserCommandQueue(IShell& shell) : shell(shell) {}

  bool enqueueWrite(int lba, unsigned int data) {
    if (isQueueFull()) {
      if (flush() == false) return false;
    }

    commands.push_back({CommandType::Write, lba, data, ""});
    return true;
  }

  bool enqueueRead(int lba, unsigned int expectedResult) {
    if (isQueueFull()) {
      if (flush() == false) return false;
    }

    string expectedOutput =
        "LBA " + std::to_string(lba) + " " + toHexString(expectedResult);
    commands.push_back({CommandType::Read, lba, 0, expectedOutput});
    return true;
  }

  bool flush() {
    if (commands.empty()) return true;
    bool result = run();
    commands.clear();
    return result;
  }

  bool isQueueFull() { return commands.size() >= maxQueueSize; }

 protected:
  IShell& shell;
  vector<ShellCommand> commands;
  const int maxQueueSize = 1000;
  bool run() {
    for (const auto& cmd : commands) {
      if (cmd.type == CommandType::Write) {
        shell.executeCommand("write " + std::to_string(cmd.lba) + " " +
                             toHexString(cmd.data));

      } else if (cmd.type == CommandType::Read) {
        shell.executeCommand("read " + std::to_string(cmd.lba));
        if (shell.getOutput() != cmd.expectedOutput) {  // read compare
          return false;
        }
      }
    }
    return true;
  }
};

class UserCommandQueueMock : public UserCommandQueue {
 public:
  using UserCommandQueue::UserCommandQueue;
  void expectAll(MockShell& mockShell) {
    Sequence readSeq, outputSeq;
    for (const auto& cmd : commands) {
      if (cmd.type == CommandType::Write) {
        EXPECT_CALL(mockShell,
                    executeCommand("write " + std::to_string(cmd.lba) + " " +
                                   toHexString(cmd.data)))
            .Times(1);
      } else {
        EXPECT_CALL(mockShell,
                    executeCommand("read " + std::to_string(cmd.lba)))
            .InSequence(readSeq);
        EXPECT_CALL(mockShell, getOutput())
            .InSequence(outputSeq)
            .WillOnce(::testing::Return(cmd.expectedOutput));
      }
    }
  }
};