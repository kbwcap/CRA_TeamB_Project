#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <iomanip>

class VirtualSSD {
public:
    VirtualSSD() {
        for (int i = 0; i < 100; ++i) {
            storage[i] = 0;
        }
        nand_file = "ssd_nand.txt";
        out_file = "ssd_output.txt";
        readFromFile(nand_file);
    }

    bool executeCommand(char cmd, int lba, unsigned int dataHex) {

        bool ret = true;

        if (cmd == 'W' || cmd == 'w') {
            ret = write(lba, dataHex);
        }
        else {
            std::cout << "Unsupported command!" << std::endl;
            ret = false;
        }

        return ret;
    }

private:
    uint32_t storage[100];
    std::string nand_file;
    std::string out_file;


    bool write(int lba, uint32_t data) {
        if (lba < 0 || lba >= 100) {
            std::cout << "LBA out of range!" << std::endl;
            return false;
        }

        if (data != 0) {
            storage[lba] = data;
            writeToFile(nand_file);
        }
    }

    bool writeToFile(std::string file_name) {
        std::ofstream outFile(file_name, std::ios::trunc);

        if (outFile.is_open()) {
            for (int lba = 0; lba < 100; ++lba) {
                if (storage[lba]) {
                    outFile << std::dec << lba << " 0x"
                        << std::uppercase << std::hex
                        << std::setw(8)
                        << std::setfill('0')
                        << storage[lba] << std::endl;
                }
            }
            outFile.close();
        }
        else {
            std::cout << "Error opening file!" << std::endl;
            return false;
        }
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

            if (iss >> index >> hexValue) {
                unsigned int value = 0;
                std::stringstream(hexValue) >> std::hex >> value;


                storage[index] = value;


                std::cout << "A[" << index << "] = 0x" << std::hex << value << std::endl;
            }
        }
    }
};
