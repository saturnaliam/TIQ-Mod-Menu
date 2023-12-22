#include "tiq.h"

void Game::setLevelAddress() {
  int pointerOffsets[7] = {0xC95B64, 0x24, 0xA8C, 0x4, 0x2C, 0x50, 0x264};
  int temp = (int)GetModuleHandle(NULL);
  for (int i = 0; i < 7; i++) temp = *(int*)(temp + pointerOffsets[i]);

  this->levelAddress = (int*)(temp + 0x4C);
}
