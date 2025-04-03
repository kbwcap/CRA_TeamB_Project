#pragma once

#include "ICommand.h"

class EraseCommand : public ICommand {
public:
  EraseCommand(VirtualSSD& ssd, int lba, int size);  // 생성자 선언만
  bool execute() override;  // execute 함수 선언만

private:
  int lba;
  int size;
};
