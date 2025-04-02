#include "gmock/gmock.h"
#include "iostream"

int main(int argc, char* argv[]) {
#ifdef _DEBUG
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
#endif  // DEBUG
	return 0;
}
