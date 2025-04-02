#include "VirtualSSD.cpp"
#include "gmock/gmock.h"
#include "iostream"

TEST(SSDTEST, basic_SSD_test_Write_1) {
  char c = 'W';
  int num = 3;
  uint32_t hexValue = 0xAAAABBBB;
  VirtualSSD ssd;
  bool ret = ssd.executeCommand(c, num, hexValue);
  EXPECT_TRUE(ret);
}

TEST(SSDTEST, basic_SSD_test_Write_2) {
  char c = 'W';
  int num = 3;
  uint32_t hexValue = 0xAAAABBBB;
  VirtualSSD ssd;
  bool ret = ssd.executeCommand(c, num, hexValue);
  num = 97;
  hexValue = 0x1234ABCD;
  ret = ssd.executeCommand(c, num, hexValue);
  EXPECT_TRUE(ret);
}

TEST(SSDTEST, basic_SSD_test_Read_3_TRUE) {
  char c = 'R';
  int num = 3;
  uint32_t hexValue = 0xAAAABBBB;
  VirtualSSD ssd;
  bool ret = ssd.executeCommand(c, num, hexValue);
  EXPECT_TRUE(ret);
}

TEST(SSDTEST, basic_SSD_test_Read_3_FALSE) {
  char c = 'R';
  int num = 3;
  uint32_t hexValue = 0x00000000;
  VirtualSSD ssd;
  bool ret = ssd.executeCommand(c, num, hexValue);
  EXPECT_FALSE(ret);
}

TEST(SSDTEST, basic_SSD_test_Read_3_OutOfRange) {
  char c = 'R';
  int num = 103;
  uint32_t hexValue = 0x00000000;
  VirtualSSD ssd;
  bool ret = ssd.executeCommand(c, num, hexValue);
  EXPECT_FALSE(ret);
}

TEST(SSDTEST, basic_SSD_test_Write_Wrong_lba_index) {
  char c = 'W';
  int num = 102;
  uint32_t hexValue = 0xAAAABBBB;
  VirtualSSD ssd;
  bool ret = ssd.executeCommand(c, num, hexValue);
  EXPECT_FALSE(ret);
}