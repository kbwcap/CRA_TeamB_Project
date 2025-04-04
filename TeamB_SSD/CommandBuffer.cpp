#include "CommandBuffer.h"

#include <windows.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "VirtualSSD.h"

namespace fs = std::filesystem;

CommandBuffer::CommandBuffer(VirtualSSD& ssd, const std::string& bufferFile,
                             const std::string& bufferFolder)
    : ssd(ssd),
      commandCount(0),
      bufferFile(bufferFile),
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

void CommandBuffer::removeSameCommmand(std::shared_ptr<ICommand> command) {
  auto receiveCommand = dynamic_cast<WriteCommand*>(command.get());
  for (int i = 0; i < commandCount; i++) {
    if (auto writeCommand = dynamic_cast<WriteCommand*>(commandBuffer[i].get())) {
      if (writeCommand->getLBA() == receiveCommand->getLBA()) {
        commandBuffer[i] = nullptr;
        updateBufferFile(i, EMPTY, 0, 0);
        return;
      }
    }
  }
}

void CommandBuffer::addCommand(std::shared_ptr<ICommand> command) {
  if (commandCount >= MAX_COMMANDS) {
    executeCommand();
    clear();
  }
  // Ignore Command
  removeSameCommmand(command);

  commandBuffer[commandCount++] = command;
}

void CommandBuffer::reloadFromCommandFile() {
  std::ifstream file(bufferFile);

  if (!file.is_open()) return;

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string commandType;
    int lba;
    uint32_t dataOrSize;

    if (iss >> commandType >> lba >> dataOrSize) {
      std::shared_ptr<ICommand> command = nullptr;
      if (commandType == WRITE) {
        command = std::make_shared<WriteCommand>(ssd, lba, dataOrSize);
        addCommand(command);
      } else if (commandType == ERASE) {
        command = std::make_shared<EraseCommand>(ssd, lba, dataOrSize);
        addCommand(command);
      }
    }
  }
  file.close();
  clearCommandFile();
  executeCommand();
}

void CommandBuffer::saveCommandToFile() {
  std::ofstream file(bufferFile, std::ios::trunc);
  if (file.is_open()) {
    for (int i = 0; i < commandCount; ++i) {
      std::stringstream ss;
      if (auto writeCommand =
              dynamic_cast<WriteCommand*>(commandBuffer[i].get())) {
        ss << "W " << writeCommand->getLBA() << " " << writeCommand->getData()
           << std::endl;
        updateBufferFile(i, WRITE, writeCommand->getLBA(),
                         writeCommand->getData());
      } else if (auto eraseCommand =
                     dynamic_cast<EraseCommand*>(commandBuffer[i].get())) {
        ss << "E " << eraseCommand->getLBA() << " " << eraseCommand->getSize()
           << std::endl;
        updateBufferFile(i, ERASE, eraseCommand->getLBA(),
                         eraseCommand->getSize());
      }
      file << ss.str();
    }
    file.close();
  }
}

void CommandBuffer::clear() {
  commandCount = 0;
  for (int i = 0; i < MAX_COMMANDS; ++i) {
    commandBuffer[i] = nullptr;
    updateBufferFile(i, EMPTY, 0, 0);
  }
  clearCommandFile();
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