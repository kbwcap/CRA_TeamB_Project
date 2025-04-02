#include "gmock/gmock.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>  // for stringstream
#include <iomanip>  // for hex formatting
#include <cctype> 
#include <algorithm>

using namespace std;

void writeOuputFile(string outData) {
  ofstream outputFile("ssd_output.txt", ios::out);
  outputFile << outData << endl;
  outputFile.close();
}

void read(int LBA) {
    // 1. ssd_nane.txt 파일에서 값 읽어 들이기
    ifstream inputFile("ssd_nand.txt");
    string line;
    vector<pair<int, unsigned int>> values; 
    while (getline(inputFile, line)) {
      int index;
      int value;
      stringstream ss(line);
      ss >> index;
      ss >> hex >> value;
      values.push_back(make_pair(index, value));
    }
    inputFile.close();

    // 2. 0 ~ 99 범위 벗어나는지 체크 
    if (LBA < 0  || LBA > 99) {
      writeOuputFile("ERROR");
      return;
    }
 
    // 3. 기록된 값이 없으면 0x00000000 값 리턴
    bool found = false;
    pair<int, unsigned int> findData; 
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
    stringstream ss;
    ss << "LBA " << findData.first << " 0x" << setfill('0') << setw(8) << hex << uppercase << findData.second;
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
  // 프로그램 시작할때 ssd_nand.txt 파일이 없으면 생성해주기.
  ofstream outputFile("ssd_nand.txt", ios::app);
  outputFile.close();


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

    // add write function

  }
  else if (c == 'R') {
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