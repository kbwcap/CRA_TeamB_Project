#pragma once

#include "ICommand.h"

class WriteCommand : public ICommand {
public:
  WriteCommand(VirtualSSD& ssd, int lba, uint32_t data);  // 생성자 선언만
  bool execute() override;  // execute 함수 선언만

private:
  int lba;
  uint32_t data;
};
