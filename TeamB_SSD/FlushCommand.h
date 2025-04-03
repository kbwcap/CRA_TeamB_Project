#pragma once

#include "ICommand.h"

class FlushCommand : public ICommand {
public:
  FlushCommand(VirtualSSD& ssd);  // 생성자 선언만
  bool execute() override;  // execute 함수 선언만
};
