#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

constexpr const char* ERROR = "ERROR";
constexpr const char* INVALID = "INVALID";
constexpr const char* NULL_VALUE = "0x00000000";
constexpr const int MAX_RANGE_NUM = 100;

class VirtualSSD {
 public:
  VirtualSSD(const std::string& nand = "ssd_nand.txt",
             const std::string& out = "ssd_output.txt")
      : nand_file(nand), out_file(out) {
    for (int i = 0; i < MAX_RANGE_NUM; ++i) {
      storage[i] = 0;
    }
    readFromFile(nand_file);
  }

  bool executeCommand(char cmd, int lba, unsigned int dataHex) {
    if (cmd == 'W' || cmd == 'w') {
      return write(lba, dataHex);
    } else if (cmd == 'R' || cmd == 'r') {
      read(lba);
      return (dataHex == storage[lba]);
    } else {
      std::cout << "Unsupported command!" << std::endl;
      return false;
    }
  }

 private:
  uint32_t storage[MAX_RANGE_NUM];
  const std::string nand_file;
  const std::string out_file;

  bool write(int lba, uint32_t data) {
    if (isOutofRange(lba)) return false;

    if (data != 0) {
      storage[lba] = data;
      writeToFile(nand_file);
    }
    return true;
  }

  bool isOutofRange(int lba) {
    if (lba < 0 || lba >= MAX_RANGE_NUM) {
      writeOuputFile(ERROR);
      std::cout << "LBA out of range!" << std::endl;
      return true;
    }
    return false;
  }

  bool writeToFile(std::string file_name) {
    std::ofstream outFile(file_name, std::ios::trunc);

    if (outFile.is_open()) {
      for (int lba = 0; lba < MAX_RANGE_NUM; ++lba) {
        if (storage[lba]) {
          outFile << std::dec << lba << " 0x" << std::uppercase << std::hex
                  << std::setw(8) << std::setfill('0') << storage[lba]
                  << std::endl;
        }
      }
      outFile.close();
    } else {
      std::cout << "Error opening file!" << std::endl;
      return false;
    }
    return true;
  }

  void readFromFile(std::string file_name) {
    std::ifstream file(file_name);

    if (!file.is_open()) {
      return;
    }

    std::string line;
    while (std::getline(file, line)) {
      std::istringstream iss(line);

      unsigned int index;
      std::string hexValue;

      if (iss >> std::dec >> index >> std::hex >> hexValue) {
        unsigned int value = 0;
        std::stringstream(hexValue) >> std::hex >> value;
        storage[index] = value;
        // std::cout << "A[" << std::dec << index << "] = 0x" << std::hex <<
        // value << std::endl;
      }
    }
  }

  void writeOuputFile(std::string outData) {
    std::ofstream outputFile(out_file, std::ios::out);
    outputFile << outData << '\n';
    outputFile.close();
  }

  bool read(int lba) {
    // 1. ssd_nane.txt 파일에서 값 읽어 들이기
    readFromFile(nand_file);

    // 2. 0 ~ 99 범위 벗어나는지 체크
    if (isOutofRange(lba)) return false;

    // 3. 기록된 값이 없으면 0x00000000 값 리턴
    // 위에서 100개의 값을 생성해주어서 이 부분은 필요 없음.

    // 4. LBA 값 ssd_ouput.txt 에 적어주기.
    std::stringstream ss;
    ss << "LBA " << lba << " 0x" << std::setfill('0') << std::setw(8)
       << std::hex << std::uppercase << storage[lba];
    writeOuputFile(ss.str());

    return true;
  }
};
