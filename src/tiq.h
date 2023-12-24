#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <cstdint>
#include <vector>

//extern BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

enum Version {
  NONE,
  FL_32,
  FL_11
};

struct GameVersion {
  Version version;
  std::string title;
  std::vector<uint32_t> pointerOffsets;
};

class Game {
  private:
    static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
    void getVersion();
    void getLevelAddress();
    GameVersion detectedVersion;

  public:
    bool initialized;
    Version gameVersion;
    std::vector<uint32_t> pointerOffsets;
    uintptr_t* levelAddress;

    Game();
    void initialize();
};
