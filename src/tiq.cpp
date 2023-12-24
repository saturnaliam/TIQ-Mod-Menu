#include "tiq.h"
#include "global.h"

Game::Game() {

}

Version version = NONE;
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
  DWORD targetProcessId = GetCurrentProcessId();
  DWORD processId;
  GetWindowThreadProcessId(hwnd, &processId);

  if (processId == targetProcessId) {
    char windowTitleC[256];
    GetWindowTextA(hwnd, windowTitleC, sizeof(windowTitleC));
    std::string windowTitle = windowTitleC;

    if (windowTitle != global::menuTitle) {
      if (windowTitle == "Adobe Flash Player 32") {
        version = FL_32;
      } else if (windowTitle == "Adobe Flash Player 11") {
        version = FL_11;
      }
    }
    return FALSE;
  }

  return TRUE;
}

void Game::initialize() {
  this->getVersion();

  if (this->gameVersion != NONE) {
    this->initialized = true;

    if (this->gameVersion == FL_32) {
      this->pointerOffsets = { 0xC95B64, 0x24, 0xA8C, 0x4, 0x2C, 0x50, 0x264, 0x4C };
    } else {

    }

    this->getLevelAddress();
  }
}

void Game::getVersion() {
  if (this->gameVersion != NONE) return;
  EnumWindows(EnumWindowsProc, 0);

  this->gameVersion = version;
}

void Game::getLevelAddress() {
  int temp = reinterpret_cast<int>(GetModuleHandle(NULL));

  for (size_t i = 0; i < this->pointerOffsets.size() - 1; i++) {
    temp = *reinterpret_cast<int*>(temp + this->pointerOffsets[i]);
  }

  this->levelAddress = reinterpret_cast<int*>(temp + this->pointerOffsets.back());
}
