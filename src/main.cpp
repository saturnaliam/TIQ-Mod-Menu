#include "gui.h"
#include "tiq.h"
#include "global.h"
#include <thread>

DWORD_PTR __stdcall eject_thread(LPVOID lpParameter) {
    HMODULE hModule = reinterpret_cast<HMODULE>(lpParameter);
    FreeLibraryAndExitThread(hModule, 0);
}

int main(HMODULE hModule) {
  gui::CreateHWindow("TIQ Mod Menu", "Mod Menu Class");
  gui::CreateDevice();
  gui::CreateImGui();

  while (gui::exit) {
    gui::BeginRender();
    gui::Render();
    gui::EndRender();

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  gui::DestroyImGui();
  gui::DestroyDevice();
  gui::DestroyHWindow();

  CreateThread(0, 0, eject_thread, hModule, 0, 0);
  return 0;
}

BOOL __stdcall DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved) {
  if (dwReason == DLL_PROCESS_ATTACH) {
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)main, hModule, 0, NULL);
  }

  return TRUE;
}
