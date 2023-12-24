#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <vector>

enum Version {
  NONE,
  FL_32,
  FL_11
};

class Game {
  private:
    void getVersion();
    void getLevelAddress();

  public:
    bool initialized = false;
    Version gameVersion = NONE;
    std::vector<int> pointerOffsets;
    int* levelAddress = nullptr;

    Game();
    void initialize();
};
