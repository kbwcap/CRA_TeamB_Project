#pragma once
#include <memory>
#include <string>
#include <vector>

#include "MockShell.h"
#include "TestManager.h"
#include "iostream"
using std::string;
using std::vector;
using ::testing::Sequence;

class ICommand {
 public:
  virtual ~ICommand() = default;
  virtual bool execute(IShell& shell) = 0;
};

class WriteShellCommand : public ICommand {
 public:
  WriteShellCommand(int lba, unsigned int data) : lba_(lba), data_(data) {}

  bool execute(IShell& shell) override {
    shell.executeCommand("write " + std::to_string(lba_) + " " +
                         toHexString(data_));
    return true;
  }

  int getLba() const { return lba_; }
  unsigned int getData() const { return data_; }

 private:
  int lba_;
  unsigned int data_;
};

class ReadShellCommand : public ICommand {
 public:
  ReadShellCommand(int lba, const std::string& expectedOutput)
      : lba_(lba), expectedOutput_(expectedOutput) {}

  bool execute(IShell& shell) override {
    shell.executeCommand("read " + std::to_string(lba_));
    return shell.getOutput() == expectedOutput_;
  }

  int getLba() const { return lba_; }
  const std::string& getExpectedOutput() const { return expectedOutput_; }

 private:
  int lba_;
  std::string expectedOutput_;
};

class EraseShellCommand : public ICommand {
 public:
  EraseShellCommand(int lba, int size) : lba_(lba), size_(size) {}

  bool execute(IShell& shell) override {
    shell.executeCommand("erase " + std::to_string(lba_) + " " +
                         std::to_string(size_));
    return true;
  }

  int getLba() const { return lba_; }
  int getSize() const { return size_; }

 private:
  int lba_;
  int size_;
};

class EraseRangeShellCommand : public ICommand {
 public:
  EraseRangeShellCommand(int start, int end) : start_(start), end_(end) {}

  bool execute(IShell& shell) override {
    shell.executeCommand("erase_range " + std::to_string(start_) + " " +
                         std::to_string(end_));
    return true;
  }

  int getStart() const { return start_; }
  int getEnd() const { return end_; }

 private:
  int start_;
  int end_;
};

class UserCommandQueue {
 public:
  UserCommandQueue(IShell& shell) : shell(shell) {}

  bool enqueueWrite(int lba, unsigned int data) {
    if (isQueueFull() && !flush()) return false;
    commands.push_back(std::make_unique<WriteShellCommand>(lba, data));
    return true;
  }

  bool enqueueRead(int lba, unsigned int expectedResult) {
    if (isQueueFull() && !flush()) return false;
    string expectedOutput =
        "LBA " + std::to_string(lba) + " " + toHexString(expectedResult);
    commands.push_back(std::make_unique<ReadShellCommand>(lba, expectedOutput));
    return true;
  }

  bool enqueueErase(int lba, int size) {
    if (isQueueFull() && !flush()) return false;
    commands.push_back(std::make_unique<EraseShellCommand>(lba, size));
    return true;
  }

  bool enqueueEraseRange(int startLba, int endLba) {
    if (isQueueFull() && !flush()) return false;
    commands.push_back(
        std::make_unique<EraseRangeShellCommand>(startLba, endLba));
    return true;
  }

  bool flush() {
    if (commands.empty()) return true;
    bool result = run();
    commands.clear();
    return result;
  }

  bool isQueueFull() const { return commands.size() >= maxQueueSize; }

 protected:
  IShell& shell;
  vector<std::unique_ptr<ICommand>> commands;
  const int maxQueueSize = 1000;

  bool run() {
    for (const auto& cmd : commands) {
      if (!cmd->execute(shell)) return false;
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
      if (auto* writeCmd = dynamic_cast<WriteShellCommand*>(cmd.get())) {
        EXPECT_CALL(
            mockShell,
            executeCommand("write " + std::to_string(writeCmd->getLba()) + " " +
                           toHexString(writeCmd->getData())))
            .Times(1);

      } else if (auto* readCmd = dynamic_cast<ReadShellCommand*>(cmd.get())) {
        EXPECT_CALL(mockShell,
                    executeCommand("read " + std::to_string(readCmd->getLba())))
            .InSequence(readSeq);
        EXPECT_CALL(mockShell, getOutput())
            .InSequence(outputSeq)
            .WillOnce(::testing::Return(readCmd->getExpectedOutput()));

      } else if (auto* eraseCmd = dynamic_cast<EraseShellCommand*>(cmd.get())) {
        EXPECT_CALL(
            mockShell,
            executeCommand("erase " + std::to_string(eraseCmd->getLba()) + " " +
                           std::to_string(eraseCmd->getSize())))
            .Times(1);

      } else if (auto* rangeCmd =
                     dynamic_cast<EraseRangeShellCommand*>(cmd.get())) {
        EXPECT_CALL(mockShell,
                    executeCommand("erase_range " +
                                   std::to_string(rangeCmd->getStart()) + " " +
                                   std::to_string(rangeCmd->getEnd())))
            .Times(1);
      }
    }
  }
};
