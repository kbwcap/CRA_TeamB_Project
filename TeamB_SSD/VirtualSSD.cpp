#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdint>

class VirtualSSD;

class ICommand {
public:
  virtual ~ICommand() = default;
  virtual bool execute() = 0;

protected:
  VirtualSSD& ssd;
  int lba;

  ICommand(VirtualSSD& ssd, int lba) : ssd(ssd), lba(lba) {}
};

class VirtualSSD {
public:
  static constexpr const char* ERROR = "ERROR";
  static constexpr const char* INVALID = "INVALID";
  static constexpr const char* NULL_VALUE = "0x00000000";
  static constexpr const int MAX_RANGE_NUM = 100;

  VirtualSSD(const std::string& nand = "ssd_nand.txt", const std::string& out = "ssd_output.txt")
    : nand_file(nand), out_file(out) {
    for (int i = 0; i < MAX_RANGE_NUM; ++i) {
      storage[i] = 0;
    }
    loadStorageFromFile();
  }

  bool executeCommand(std::shared_ptr<ICommand> command) {
    return command->execute();
  }

  void setData(int lba, uint32_t data) {
    storage[lba] = data;
  }

  uint32_t getData(int lba) {
    return storage[lba];
  }

  bool saveStorageToFile() {
    std::ofstream outFile(nand_file, std::ios::trunc);

    if (outFile.is_open()) {
      for (int lba = 0; lba < MAX_RANGE_NUM; ++lba) {
        if (storage[lba]) {
          outFile << std::dec << lba << " 0x" << std::uppercase << std::hex
            << std::setw(8) << std::setfill('0') << storage[lba]
            << std::endl;
        }
      }
      outFile.close();
    }
    else {
      std::cout << "Error opening file!" << std::endl;
      return false;
    }
    return true;
  }

  void loadStorageFromFile() {
    std::ifstream file(nand_file);

    if (!file.is_open()) {
      return;
    }

    std::string line;
    while (std::getline(file, line)) {
      std::istringstream iss(line);

      uint32_t index;
      std::string hexValue;

      if (iss >> std::dec >> index >> std::hex >> hexValue) {
        uint32_t value = 0;
        std::stringstream(hexValue) >> std::hex >> value;
        storage[index] = value;
      }
    }
  }

  void saveOutputToFile(std::string outData) {
    std::ofstream outputFile(out_file, std::ios::out);
    outputFile << outData << '\n';
    outputFile.close();
  }

  bool isOutOfRange(int lba) {
    if (lba < 0 || lba >= MAX_RANGE_NUM) {
      saveOutputToFile(ERROR);
      return true;
    }
    return false;
  }

private:
  uint32_t storage[MAX_RANGE_NUM];
  const std::string nand_file;
  const std::string out_file;
};

class WriteCommand : public ICommand {
public:
  WriteCommand(VirtualSSD& ssd, int lba, uint32_t data)
    : ICommand(ssd, lba), data(data) {
  }

  bool execute() override {
    if (ssd.isOutOfRange(lba)) return false;
    ssd.setData(lba, data);
    ssd.saveStorageToFile();

    return true;
  }

private:
  uint32_t data;
};

class ReadCommand : public ICommand {
public:
  ReadCommand(VirtualSSD& ssd, int lba)
    : ICommand(ssd, lba) {
  }

  bool execute() override {
    if (ssd.isOutOfRange(lba)) return false;

    std::stringstream ss;
    ss << "LBA " << lba << " 0x" << std::setfill('0') << std::setw(8)
      << std::hex << std::uppercase << ssd.getData(lba);
    ssd.saveOutputToFile(ss.str());

    return true;
  }
};

