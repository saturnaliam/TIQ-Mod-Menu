#include "tiq.h"
#include "global.h"

void initFlashVersions() {
    // Flash Player 32 SA
    GameVersion Flash32;
    Flash32.version = FL_32;
    Flash32.title = "Adobe Flash Player 32";
    Flash32.pointerOffsets = { 0xC95B64, 0x24, 0xA8C, 0x4, 0x2C, 0x50, 0x264, 0x4C };

    // Flash Player 11 SA
    GameVersion Flash11;
    Flash11.version = FL_32;
    Flash11.title = "Adobe Flash Player 11";
    Flash11.pointerOffsets = { 0xC95B64, 0x24, 0xA8C, 0x4, 0x2C, 0x50, 0x264, 0x4C };

    global::flashVersions.push_back(Flash11);
    global::flashVersions.push_back(Flash32);
  }

Game::Game() {
  initFlashVersions();
  this->hwnd = nullptr;
  this->initialized = false;
  this->gameVersion = NONE;
  this->levelAddress = nullptr;
}

// Callback function to get the HWND of Flash Player & to detect the version
BOOL CALLBACK Game::EnumWindowsProc(HWND hwnd, LPARAM lParam) {
  DWORD targetProcessId = GetCurrentProcessId();
  DWORD processId;
  GetWindowThreadProcessId(hwnd, &processId);

  if (processId == targetProcessId) {
    char windowTitle[256];
    GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle));
    std::string windowTitle = windowTitle;

    if (windowTitle != global::menuTitle) {
      reinterpret_cast<Game*>(lParam)->hwnd = hwnd;
      for (auto it : global::flashVersions) {
        if (windowTitle == it.title) {
          reinterpret_cast<Game*>(lParam)->detectedVersion = it;
          break;
        }
      }
    }
    return FALSE;
  }

  return TRUE;
}

void Game::initialize() {
  this->getVersion();

  if (this->gameVersion != NONE) {
    if (this->hwnd == nullptr) {
      this->hwnd = FindWindowA(nullptr, this->detectedVersion.title.c_str());
    }

    this->initialized = true;

    this->pointerOffsets = this->detectedVersion.pointerOffsets;

    this->getLevelAddress();
  }
}

void Game::getVersion() {
  if (this->gameVersion != NONE) return;
  EnumWindows(this->EnumWindowsProc, reinterpret_cast<LPARAM>(this));


  this->gameVersion = this->detectedVersion.version;
}

void Game::getLevelAddress() {
  uintptr_t temp = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));

  for (size_t i = 0; i < this->pointerOffsets.size() - 1; i++) {
    temp = *reinterpret_cast<uintptr_t*>(temp + this->pointerOffsets[i]);
  }

  this->levelAddress = reinterpret_cast<uintptr_t*>(temp + this->pointerOffsets.back());
}
