#pragma once
#include "../TestManager.h"

#define REGISTER_TEST(custom_name, fn)                       \
  namespace {                                                \
  struct AutoRegister_##fn {                                 \
    AutoRegister_##fn() {                                    \
      TestManager::instance().registerTest(custom_name, fn); \
    }                                                        \
  };                                                         \
  static AutoRegister_##fn _auto_register_##fn;              \
  }