#include "gmock/gmock.h"
#include "iostream"
#include "VirtualSSD.cpp"

TEST(SSDTEST, basic_SSD_test_Write_1)
{
    char c = 'W';
    int num = 3;
    unsigned int hexValue = 0xAAAABBBB;
    VirtualSSD ssd;
    bool ret = ssd.executeCommand(c, num, hexValue);
    EXPECT_TRUE(ret);
}

TEST(SSDTEST, basic_SSD_test_Write_2)
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