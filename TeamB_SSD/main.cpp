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

int main() {
    // 프로그램 시작할때 ssd_nand.txt 파일이 없으면 생성해주기.
    ofstream outputFile("ssd_nand.txt", ios::app);
    outputFile.close();

    // 사용자에게 입력 받기
    string input;
    while (true) {
      getline(cin, input);

      // "exit" 명령어 입력 시 쉘 종료
      if (input == "exit") {
        cout << "쉘을 종료합니다." << endl;
        break;
      } 
      
      // read 호출하기
      stringstream ss(input);
      string command;
      int index = 0;
      ss >> command;    
      ss >> index;
      transform(command.begin(), command.end(), command.begin(), ::toupper);
      if (command == "R") {
        read(index);
      }
    }

    // 테스트...
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
