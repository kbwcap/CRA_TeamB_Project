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
    // 1. ssd_nane.txt ���Ͽ��� �� �о� ���̱�
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

    // 2. 0 ~ 99 ���� ������� üũ 
    if (LBA < 0  || LBA > 99) {
      writeOuputFile("ERROR");
      return;
    }
 
    // 3. ��ϵ� ���� ������ 0x00000000 �� ����
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

    // 4. LBA �� ssd_ouput.txt �� �����ֱ�.
    stringstream ss;
    ss << "LBA " << findData.first << " 0x" << setfill('0') << setw(8) << hex << uppercase << findData.second;
    writeOuputFile(ss.str());
}

int main() {
    // ���α׷� �����Ҷ� ssd_nand.txt ������ ������ �������ֱ�.
    ofstream outputFile("ssd_nand.txt", ios::app);
    outputFile.close();

    // ����ڿ��� �Է� �ޱ�
    string input;
    while (true) {
      getline(cin, input);

      // "exit" ��ɾ� �Է� �� �� ����
      if (input == "exit") {
        cout << "���� �����մϴ�." << endl;
        break;
      } 
      
      // read ȣ���ϱ�
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

    // �׽�Ʈ...
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
