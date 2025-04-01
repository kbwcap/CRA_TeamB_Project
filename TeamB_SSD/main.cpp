#include "gmock/gmock.h"
#include "iostream"
#include "VirtualSSD.cpp"

TEST(SSDTEST, basic_SSD_test_1)
{
  char c = 'W';
  int num = 3;
  unsigned int hexValue = 0xAAAABBBB;
  VirtualSSD ssd;
  bool ret = ssd.executeCommand(c, num, hexValue);
  EXPECT_TRUE(ret);
}

TEST(SSDTEST, basic_SSD_test_2)
{
    char c = 'W';
    int num = 3;
    unsigned int hexValue = 0xAAAABBBB;
    VirtualSSD ssd;
    bool ret = ssd.executeCommand(c, num, hexValue);
    num = 97;
    hexValue = 0x1234ABCD;
    ret = ssd.executeCommand(c, num, hexValue);
    EXPECT_TRUE(ret);
}

int main(int argc, char* argv[]) {
#ifdef _DEBUG
  testing::InitGoogleTest();
  return RUN_ALL_TESTS();

#else

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

    std::cout << "Read Mode: num = " << num << std::endl;

    // add  Read fucntion

  }
  else {
    std::cerr << "Invalid mode. Use 'W' for write or 'R' for read." << std::endl;
    return 1;
  }
#endif
  return 0;
}