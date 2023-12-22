#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vector>

class Game {
  public:
    int* levelAddress;

    void setLevelAddress();
};
