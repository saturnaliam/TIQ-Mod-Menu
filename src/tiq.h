#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void obtainLevelAddress(int* &address);

class Game {
  public:
    int* levelAddress;

    void setLevelAddress();
};
