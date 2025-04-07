#include "CommandBuffer.h"

#include <windows.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "VirtualSSD.h"

namespace fs = std::filesystem;

CommandBuffer::CommandBuffer(VirtualSSD& ssd,
                             const std::string& bufferFolder)
    : ssd(ssd),
      commandCount(0),
      bufferFolder(bufferFolder) {
  initializeBufferFolder();
}

CommandBuffer::~CommandBuffer() { saveCommandToFile(); }

void CommandBuffer::initializeBufferFolder() {
  DWORD dwAttrib = GetFileAttributesA(bufferFolder.c_str());
  if (dwAttrib == INVALID_FILE_ATTRIBUTES ||
      !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
    std::wstring wBufferFolder(bufferFolder.begin(), bufferFolder.end());
    if (!CreateDirectoryW(wBufferFolder.c_str(), NULL)) {
      std::cerr << "Error: Unable to create buffer folder" << std::endl;
    }
  }
}

void CommandBuffer::updateBufferFile(int index, const std::string& commandType,
                                     int lba, uint32_t dataOrSize) {
  if (index < 0) return;

  std::stringstream ss;
  ss << bufferFolder << "\\" << index + 1 << "_";

  bool fileFound = false;
  for (const auto& entry : fs::directory_iterator(bufferFolder)) {
    std::string sub_filename = ss.str();
    if (entry.is_regular_file() && entry.path().filename().string().rfind(
                                       sub_filename.substr(7, 2), 0) == 0) {
      std::string oldFileName = entry.path().string();
      std::string newFileName = ss.str();

      if (commandType == EMPTY) {
        newFileName += EMPTY;
        renameOrCreateFile(oldFileName, newFileName);
        fileFound = true;
      } else if (commandType == WRITE) {
        newFileName +=
            "W_" + std::to_string(lba) + "_" + std::to_string(dataOrSize);
        renameOrCreateFile(oldFileName, newFileName);
        fileFound = true;
      } else if (commandType == ERASE) {
        newFileName +=
            "E_" + std::to_string(lba) + "_" + std::to_string(dataOrSize);
        renameOrCreateFile(oldFileName, newFileName);
        fileFound = true;
      }
    }
  }

  if (!fileFound) {
    std::string newFileName = ss.str();
    if (commandType == EMPTY) {
      newFileName += EMPTY;
    } else if (commandType == WRITE) {
      newFileName +=
          "W_" + std::to_string(lba) + "_" + std::to_string(dataOrSize);
    } else if (commandType == ERASE) {
      newFileName +=
          "E_" + std::to_string(lba) + "_" + std::to_string(dataOrSize);
    }
    createFile(newFileName);
  }
}

void CommandBuffer::renameOrCreateFile(const std::string& oldFileName,
                                       const std::string& newFileName) {
  if (fs::exists(oldFileName)) {
    try {
      fs::rename(oldFileName, newFileName);
    } catch (const std::exception& e) {
      std::cerr << "Error renaming file: " << e.what() << std::endl;
    }
  } else {
    createFile(newFileName);
  }
}

void CommandBuffer::createFile(const std::string& newFileName) {
  fs::path filePath(newFileName);
  fs::path parentPath = filePath.parent_path();

  if (!fs::exists(parentPath)) {
    std::cout << "Creating directory: " << parentPath << std::endl;
    fs::create_directories(parentPath);
  }

  std::ofstream newFile(newFileName);
  if (newFile) {
    newFile.close();
  } else {
    std::cerr << "Error creating file: " << newFileName << std::endl;
  }
}

// 최소값 함수
template <typename T>
T my_min(const T& a, const T& b) {
  return (a < b) ? a : b;
}

// 최대값 함수
template <typename T>
T my_max(const T& a, const T& b) {
  return (a > b) ? a : b;
}


std::shared_ptr<ICommand> CommandBuffer::setIgnoreMergeCommmand(
    std::shared_ptr<ICommand> command) {
  // Case 1: WriteCommand
  if (auto rWriteCommand = dynamic_cast<WriteCommand*>(command.get())) {
    for (int i = 0; i < commandCount; ++i) {
      if (auto w = dynamic_cast<WriteCommand*>(commandBuffer[i].get())) {
        if (w->getLBA() == rWriteCommand->getLBA()) {
          removeAtBuffer(i);
          updateCommandBuffer();
          return nullptr;
        }
      }
    }
  }

  // Case 2: EraseCommand
   else if (auto rEraseCommand = dynamic_cast<EraseCommand*>(command.get())) {
   bool merged = false;
   for (int i = 0; i < commandCount; ++i) {
     if (auto w = dynamic_cast<WriteCommand*>(commandBuffer[i].get())) {
       // erase가 write 범위를 포함하면 write 제거
       if (isEraseInRange(w->getLBA(), 1, rEraseCommand->getLBA(), rEraseCommand->getSize())){
         removeAtBuffer(i);
       }
     } 
     else if (auto e = dynamic_cast<EraseCommand*>(commandBuffer[i].get())) {
       if (isEraseInRange(e->getLBA(), e->getSize(), rEraseCommand->getLBA(), rEraseCommand->getSize())) {
         removeAtBuffer(i);
         continue;
       }
       else if(canMerge(rEraseCommand->getLBA(), rEraseCommand->getSize(), e->getLBA(), e->getSize())) {
         removeAtBuffer(i);
         int mergedLBA = my_min(rEraseCommand->getLBA(), e->getLBA());
         int mergedEnd =
             my_max(rEraseCommand->getLBA() + rEraseCommand->getSize() - 1,
                      e->getLBA() + e->getSize() - 1);
         int mergedSize = mergedEnd - mergedLBA + 1;
         //mergedSize = mergedSize > 10 ? 10 : mergedSize;
         
         // size가 10 초과일 경우 10개씩 잘라서 추가
         if (mergedSize > 10) {
           int offset = 0;
           for (; offset + 10 <= mergedSize; offset += 10) {
             int chunkLBA = mergedLBA + offset;
             int chunkSize = 10;
             auto chunkCommand =
                 std::make_shared<EraseCommand>(ssd, chunkLBA, chunkSize);
             addCommand(chunkCommand);
           }

           updateCommandBuffer();

           // 마지막 남은 조각이 있을 경우 -> return
           int remaining = mergedSize - offset;
           if (remaining > 0) {
             int chunkLBA = mergedLBA + offset;
             int chunkSize = remaining;
             return std::make_shared<EraseCommand>(ssd, chunkLBA, chunkSize);
           }

           // 다 나눠 넣었으면 아무 것도 반환하지 않음
           return nullptr;
         } else {
           updateCommandBuffer();
           command =
               std::make_shared<EraseCommand>(ssd, mergedLBA, mergedSize);
           merged = true;
         }           
       }
     }
   }

   updateCommandBuffer();
   return merged ? command : nullptr;
 }

  return nullptr;
}

void CommandBuffer::updateCommandBuffer() {
  int writeLBA = 0;

  for (int readLBA = 0; readLBA < commandCount; ++readLBA) {
    if (commandBuffer[readLBA] != nullptr) {
      if (writeLBA != readLBA) {
        commandBuffer[writeLBA] = commandBuffer[readLBA];
      }
      ++writeLBA;
    }
  }

  for (int i = writeLBA; i < commandCount; ++i) {
    commandBuffer[i] = nullptr;
  }
  commandCount = writeLBA;
}

void CommandBuffer::removeAtBuffer(int lba) {
  commandBuffer[lba] = nullptr;
  updateBufferFile(lba, EMPTY, 0, 0);
}

bool CommandBuffer::isEraseInRange(int innerId, int innerSize, int outerId,
                                 int outerSize) {
  int innerStart = innerId;
  int innerEnd = innerId + innerSize - 1;

  int outerStart = outerId;
  int outerEnd = outerId + outerSize - 1;

  return outerStart <= innerStart && innerEnd <= outerEnd;
}

bool CommandBuffer::canMerge(int id1, int size1, int id2, int size2) {
  int start1 = id1;
  int end1 = id1 + size1 - 1;
  int start2 = id2;
  int end2 = id2 + size2 - 1;

  // 겹치거나 붙어 있는 경우
  return !(end1 < start2 || end2 < start1);
}

void CommandBuffer::addCommand(std::shared_ptr<ICommand> command) {
  if (commandCount >= MAX_COMMANDS) {
    executeCommand();
    ssd.saveStorageToFile();
    clear();
  }

  // Ignore & Merge Command - Write, Erase
  auto mergedCommand = setIgnoreMergeCommmand(command);
  if (mergedCommand == nullptr)
    commandBuffer[commandCount++] = command;
  else {
    commandBuffer[commandCount++] = mergedCommand;
  }
}

void CommandBuffer::reloadFromBufferFolderCommandFile() {
  std::stringstream ss;
  ss << bufferFolder << "\\";

  for (const auto& entry : fs::directory_iterator(bufferFolder)) {

    if (entry.is_regular_file()) {
      std::string oldFileName = entry.path().string();

      size_t pos = oldFileName.find('\\');
      if (pos == std::string::npos) {
        return;
      }
      std::string payload = oldFileName.substr(pos + 1);

      std::vector<std::string> tokens;
      std::stringstream ss1(payload);
      std::string token;

      while (std::getline(ss1, token, '_')) {
        tokens.push_back(token);
      }

      if (tokens.size() >= 4) {
        std::string commandType = tokens[1];
        int lba = std::stoi(tokens[2]);
        long long dataOrSize = std::stoll(tokens[3]);
        std::shared_ptr<ICommand> command = nullptr;
        if (commandType == "W") {
          command = std::make_shared<WriteCommand>(ssd, lba, dataOrSize);
          addCommand(command);
        }
        else if (commandType == "E") {
          command = std::make_shared<EraseCommand>(ssd, lba, dataOrSize);
          addCommand(command);
        }
      }
    }
  }
  executeCommand();
}

void CommandBuffer::saveCommandToFile() {
  for (int i = 0; i < commandCount; ++i) {
    if (auto writeCommand =
            dynamic_cast<WriteCommand*>(commandBuffer[i].get())) {
      updateBufferFile(i, WRITE, writeCommand->getLBA(),
                        writeCommand->getData());
    } else if (auto eraseCommand =
                    dynamic_cast<EraseCommand*>(commandBuffer[i].get())) {
      updateBufferFile(i, ERASE, eraseCommand->getLBA(),
                        eraseCommand->getSize());
    }
  }
}

void CommandBuffer::clear() {
  commandCount = 0;
  for (int i = 0; i < MAX_COMMANDS; ++i) {
    commandBuffer[i] = nullptr;
    updateBufferFile(i, EMPTY, 0, 0);
  }
}

void CommandBuffer::clearCommandFile() {
  std::ofstream file(bufferFile, std::ios::trunc);
  if (file.is_open()) {
    file.close();
  } else {
    std::cerr << "Error: Unable to open file " << bufferFile << " for clearing."
              << std::endl;
  }
}

void CommandBuffer::executeCommand() {
  for (int i = 0; i < commandCount; ++i) {
    commandBuffer[i]->execute();
  }
}

bool CommandBuffer::getReadCommandBuffer(int lba, uint32_t& data) {
  if (commandCount == 0) return false;

  for (int i = 0; i < commandCount; i++) {
    if (auto writeCommand = dynamic_cast<WriteCommand*>(commandBuffer[i].get())) {
      if (writeCommand->getLBA() == lba) {
        data = writeCommand->getData();
        return true;
      }
    }
  }
  return false;
}

int CommandBuffer::getBufferSize() const { return commandCount; }
