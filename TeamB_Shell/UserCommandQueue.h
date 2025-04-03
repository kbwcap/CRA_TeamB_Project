#pragma once
#include <string>
#include <vector>

#include "MockShell.h"
#include "TestManager.h"
#include "iostream"
using std::string;
using std::vector;
using ::testing::Sequence;

enum class CommandType { Write, Read, Erase, Erase_Range };

struct ShellCommand {
  CommandType type;
  int startLba;
  int endLba;                  // Only for erase_range
  int size;                    // Only for erase
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

    commands.push_back({CommandType::Write, lba, 0, 0, data, ""});
    return true;
  }

  bool enqueueRead(int lba, unsigned int expectedResult) {
    if (isQueueFull()) {
      if (flush() == false) return false;
    }

    string expectedOutput =
        "LBA " + std::to_string(lba) + " " + toHexString(expectedResult);
    commands.push_back({CommandType::Read, lba, 0, 0, 0, expectedOutput});
    return true;
  }

  bool enqueueErase(int lba, int size) {
    if (isQueueFull()) {
      if (flush() == false) return false;
    }

    commands.push_back({CommandType::Erase, lba, 0, size, 0, ""});
    return true;
  }

  bool enqueueEraseRange(int startLba, int endLba) {
    if (isQueueFull()) {
      if (flush() == false) return false;
    }

    commands.push_back({CommandType::Erase_Range, startLba, endLba, 0, 0, ""});
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
        shell.executeCommand("write " + std::to_string(cmd.startLba) + " " +
                             toHexString(cmd.data));

      } else if (cmd.type == CommandType::Read) {
        shell.executeCommand("read " + std::to_string(cmd.startLba));
        if (shell.getOutput() != cmd.expectedOutput) {  // read compare
          return false;
        }
      } else if (cmd.type == CommandType::Erase) {
        shell.executeCommand("erase " + std::to_string(cmd.startLba) + " " +
                             std::to_string(cmd.size));
      } else if (cmd.type == CommandType::Erase_Range) {
        shell.executeCommand("erase_range " + std::to_string(cmd.startLba) +
                             " " + std::to_string(cmd.endLba));
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
                    executeCommand("write " + std::to_string(cmd.startLba) +
                                   " " + toHexString(cmd.data)))
            .Times(1);
      } else if (cmd.type == CommandType::Read) {
        EXPECT_CALL(mockShell,
                    executeCommand("read " + std::to_string(cmd.startLba)))
            .InSequence(readSeq);
        EXPECT_CALL(mockShell, getOutput())
            .InSequence(outputSeq)
            .WillOnce(::testing::Return(cmd.expectedOutput));
      } else if (cmd.type == CommandType::Erase) {
        EXPECT_CALL(mockShell,
                    executeCommand("erase " + std::to_string(cmd.startLba) +
                                   " " + std::to_string(cmd.size)))
            .Times(1);
      } else if (cmd.type == CommandType::Erase_Range) {
        EXPECT_CALL(mockShell,
                    executeCommand("erase_range " + std::to_string(cmd.startLba) +
                                   " " + std::to_string(cmd.endLba)))
            .Times(1);
      }
    }
  }
};