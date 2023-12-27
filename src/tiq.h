#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <cstdint>
#include <vector>

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
    Version gameVersion;
    std::vector<uint32_t> pointerOffsets;
    static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
    void getVersion();
    void getLevelAddress();
    void getWindowCoordinates();
    void getCursorCoordinates();

  public:
    HWND hwnd;
    int windowCoordinates[2] = { 0, 0 };
    int mouseCoordinates[2] = { 0, 0 };
    GameVersion detectedVersion;
    uintptr_t* levelAddress;
    bool initialized;

    Game();
    void initialize();
    void update();
};
