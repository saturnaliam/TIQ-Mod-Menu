#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vector>

class Game {
  public:
    std::vector<int> pointerOffsets;
    int* levelAddress;

    Game();
    void setLevelAddress();
};
