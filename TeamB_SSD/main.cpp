#include "gmock/gmock.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>  // for stringstream
#include <iomanip>  // for hex formatting
#include <cctype> 
#include <algorithm>
#include "VirtualSSD.cpp"

void writeOuputFile(std::string outData) {
  std::ofstream outputFile("ssd_output.txt", std::ios::out);
  outputFile << outData << '\n';
  outputFile.close();
}

void read(int LBA) {
  // 1. ssd_nane.txt 파일에서 값 읽어 들이기
  std::ifstream inputFile("ssd_nand.txt");
  std::string line;
  std::vector<std::pair<int, unsigned int>> values;
  while (getline(inputFile, line)) {
    int index;
    int value;
    std::stringstream ss(line);
    ss >> index;
    ss >> std::hex >> value;
    values.push_back(std::make_pair(index, value));
  }
  inputFile.close();

  // 2. 0 ~ 99 범위 벗어나는지 체크 
  if (LBA < 0 || LBA > 99) {
    writeOuputFile("ERROR");
    return;
  }

  // 3. 기록된 값이 없으면 0x00000000 값 리턴
  bool found = false;
  std::pair<int, unsigned int> findData;
  for (const auto& pair : values) {
    if (pair.first == LBA) {
      found = true;
      findData = pair;
      break;
    }
  }
  if (found == false) {
    writeOuputFile("0x00000000");
    return;
  }

  // 4. LBA 값 ssd_ouput.txt 에 적어주기.
  std::stringstream ss;
  ss << "LBA " << findData.first << " 0x" << std::setfill('0') << std::setw(8) << std::hex << std::uppercase << findData.second;
  writeOuputFile(ss.str());
}

TEST(SSDTEST, basic_test)
{
  char c = 2;                            // W or R
  int num = 3;                          // 2
  unsigned int hexValue = 0xAAAABBBB;
  EXPECT_EQ(2, 2);
}

int main(int argc, char* argv[]) {
#ifdef _DEBUG
  testing::InitGoogleTest();
  return RUN_ALL_TESTS();

#else
   VirtualSSD ssd;

  if (argc < 2 || argc > 4) {
    std::cerr << "Usage: ssd.exe W <int> <hexadecimal>  or  ssd.exe R <int>" << std::endl;
    return 1;
  }

  char c = argv[1][0];

  if (c == 'W') {
    if (argc != 4) {
      std::cerr << "For 'W' mode, provide 2 arguments: <int> <hexadecimal>" << std::endl;
      return 1;
    }

    int num = std::atoi(argv[2]);

    unsigned int hexValue = std::strtoul(argv[3], nullptr, 16);

    std::cout << "Write Mode: num = " << num << ", hexValue = 0x" << std::hex << hexValue << std::endl;

    ssd.executeCommand(c, num, hexValue);

  } else if (c == 'R') {
    if (argc != 3) {
      std::cerr << "For 'R' mode, provide 1 argument: <int>" << std::endl;
      return 1;
    }

    int num = std::atoi(argv[2]);

    // read 호출하기
    read(num);
    
    //std::cout << "Read Mode: num = " << num << std::endl;

    // add  Read fucntion

  }
  else {
    std::cerr << "Invalid mode. Use 'W' for write or 'R' for read." << std::endl;
    return 1;
  }
#endif
  return 0;
}