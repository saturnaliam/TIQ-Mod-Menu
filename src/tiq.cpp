#include "tiq.h"

Game::Game() {
  this->pointerOffsets = { 0xC95B64, 0x24, 0xA8C, 0x4, 0x2C, 0x50, 0x264, 0x4C };
}

void Game::setLevelAddress() {
  int temp = reinterpret_cast<int>(GetModuleHandle(NULL));

  for (int i = 0; i < this->pointerOffsets.size() - 1; i++) {
    temp = *reinterpret_cast<int*>(temp + this->pointerOffsets[i]);
  }

  this->levelAddress = reinterpret_cast<int*>(temp + this->pointerOffsets.back());
}
