#include "gui.h"

#include <format>

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"
#include "tiq.h"
#include "global.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter);

long __stdcall WindowProcess(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter) {
  if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter)) {
    return true;
  }

  switch (message) {
    case WM_SIZE: {
      if (gui::device && wideParameter != SIZE_MINIMIZED) {
        gui::presentParameters.BackBufferHeight = LOWORD(longParameter);
        gui::presentParameters.BackBufferWidth = HIWORD(longParameter);
        gui::ResetDevice();
      }
    } return 0;

    case WM_SYSCOMMAND: {
      if ((wideParameter & 0xFFF0) == SC_KEYMENU) {
        return 0;
      }
    } break;

    case WM_DESTROY: {
      PostQuitMessage(0);
    } return 0;

    case WM_LBUTTONDOWN: {
      gui::position = MAKEPOINTS(longParameter);
    } return 0;

    case WM_MOUSEMOVE: {
      if (wideParameter == MK_LBUTTON) {
        const auto points = MAKEPOINTS(longParameter);
        auto rect = ::RECT{ };

        GetWindowRect(gui::window, &rect);

        rect.left += points.x - gui::position.x;
        rect.top += points.y - gui::position.y;

        if (gui::position.x >= 0 && gui::position.x <= gui::WIDTH && gui::position.y >= 0 && gui::position.y <= 19) {
          SetWindowPos(gui::window, HWND_TOPMOST, rect.left, rect.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
        }
      }
    } return 0;
  }

  return DefWindowProcW(window, message, wideParameter, longParameter);
}

void gui::CreateHWindow(const char* windowName, const char* className) noexcept {
  windowClass.cbSize = sizeof(WNDCLASSEXA);
  windowClass.style = CS_CLASSDC;
  windowClass.lpfnWndProc = WindowProcess;
  windowClass.cbClsExtra = 0;
  windowClass.cbWndExtra = 0;
  windowClass.hInstance = GetModuleHandleA(0);
  windowClass.hIcon = 0;
  windowClass.hCursor = 0;
  windowClass.hbrBackground = 0;
  windowClass.lpszMenuName = 0;
  windowClass.lpszClassName = className;
  windowClass.hIconSm = 0;

  RegisterClassExA(&windowClass);

  window = CreateWindowA(className, windowName, WS_POPUP, 100, 100, WIDTH, HEIGHT, 0, 0, windowClass.hInstance, 0);

  ShowWindow(window, SW_SHOWDEFAULT);
  UpdateWindow(window);
}

void gui::DestroyHWindow() noexcept {
  DestroyWindow(window);
  UnregisterClassA(windowClass.lpszClassName, windowClass.hInstance);
}

bool gui::CreateDevice() noexcept {
  d3d = Direct3DCreate9(D3D_SDK_VERSION);

  if (!d3d) {
    return false;
  }

  ZeroMemory(&presentParameters, sizeof(presentParameters));

  presentParameters.Windowed = TRUE;
  presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
  presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
  presentParameters.EnableAutoDepthStencil = TRUE;
  presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
  presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

  if (d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &presentParameters, &device) < 0) {
    return false;
  }

  return true;
}

void gui::ResetDevice() noexcept {
  ImGui_ImplDX9_InvalidateDeviceObjects();

  const auto result = device->Reset(&presentParameters);

  if (result == D3DERR_INVALIDCALL) {
    IM_ASSERT(0);
  }

  ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::DestroyDevice() noexcept {
  if (device) {
    device->Release();
    device = nullptr;
  }

  if (d3d) {
    d3d->Release();
    d3d = nullptr;
  }
}

void gui::CreateImGui() noexcept {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ::ImGui::GetIO();

  io.IniFilename = NULL;

  ImGui::StyleColorsClassic();
  ImGuiStyle& style = ImGui::GetStyle();
  style.FrameRounding = 7;

  ImGui_ImplWin32_Init(window);
  ImGui_ImplDX9_Init(device);
}

void gui::DestroyImGui() noexcept {
  ImGui_ImplDX9_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();
}

void gui::BeginRender() noexcept {
  MSG message;
  while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
    TranslateMessage(&message);
    DispatchMessage(&message);
  }

  ImGui_ImplDX9_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();
}

void gui::EndRender() noexcept {
  ImGui::EndFrame();

  device->SetRenderState(D3DRS_ZENABLE, FALSE);
  device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

  device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

  if (device->BeginScene() >= 0) {
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    device->EndScene();
  }

  const auto result = device->Present(0, 0, 0, 0);

  if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
    ResetDevice();
  }
}

void gui::Render() noexcept {
  global::game.update();

  ImGui::SetNextWindowPos({0, 0});
  ImGui::SetNextWindowSize({WIDTH, HEIGHT});
  ImGui::Begin(std::format("{} v{}", global::menuTitle, global::menuVersion).c_str(), &exit, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar);

  ImGuiTabBarFlags tabFlags = ImGuiTabBarFlags_None;

  if (creditsOpened) {
    renderCredits();
  }

  renderMenu();

  if (global::game.initialized) {
    if (ImGui::BeginTabBar("TabBar", tabFlags)) {
      if (ImGui::BeginTabItem("Game Info")) {
        renderMemorySection();

        renderValuesSection();

        ImGui::EndTabItem();
      }

      if(ImGui::BeginTabItem("Rich Presence")) {
        ImGui::EndTabItem();
      }

      if(ImGui::BeginTabItem("Level Chooser")) {
        ImGui::EndTabItem();
      }

      ImGui::EndTabItem();

      ImGui::EndTabBar();
    }
  } else {
    // Detection of Flash Player version
    ImGui::OpenPopup("Detecting Flash");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Detecting Flash", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
      ImGui::SeparatorText("Detecting Flash Version");

      const char* flash_versions[] = {"Flash Player 32", "Flash Player 11"};
      static int current_version = -1;
      ImGui::Combo("Flash Version", &current_version, flash_versions, IM_ARRAYSIZE(flash_versions));
      ImGui::SetItemTooltip("Select the current version of Flash Player\nor click on the Flash Player window.");

      if (current_version != -1) {
        global::game.detectedVersion = global::flashVersions[current_version];
      }

      global::game.initialize();

      ImGui::EndPopup();
    }
  }

  ImGui::End();
}

void gui::renderCredits() {
  ImGui::OpenPopup("Credits");

  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

  if (ImGui::BeginPopupModal("Credits", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Menu & mods created by Lucia (https://github.com/saturnaliam)");
    // TODO Add credits for Discord SDK, ImGui, and anything else that happens to come up
    ImGui::Text("DISCORD SDK CREDITS HERE");
    ImGui::Text("Made with Dear ImGui (https://github.com/ocornut/imgui)");

    if (ImGui::Button("Close")) {
      creditsOpened = false;
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
 }
}

void gui::renderMenu() {
  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("About")) {
      ImGui::MenuItem(std::format("Ananke v{}", global::menuVersion).c_str(), NULL, false, false);
      ImGui::MenuItem("Credits", NULL, &creditsOpened);
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
}

void gui::renderMemorySection() {
  ImGui::SeparatorText("Memory");
  ImGui::IntBox("Level Address", reinterpret_cast<int>(global::game.levelAddress), "0x%X");

  if (ImGui::CollapsingHeader("Pointer Offsets")) {
    for (auto it : global::game.detectedVersion.pointerOffsets) {
      ImGui::BulletText("0x%X", it);
    }
  }
}

void gui::renderValuesSection() {
  ImGui::SeparatorText("Game Values");

  ImGui::IntBox("Current Scene", *global::game.levelAddress);

  if (global::game.hwnd == nullptr) {
    ImGui::BeginDisabled();
    ImGui::Text("Bring the Flash window into focus! (Error 01)");
  }
  ImGui::IntBoxHalf("Relative Mouse Coordinates", global::game.mouseCoordinates);

  ImGui::IntBoxHalf("Window Coordinates", global::game.windowCoordinates);
  if (global::game.hwnd == nullptr) { ImGui::EndDisabled(); }
}

inline void ImGui::IntBox(const char* label, int v, const char* format) {
  ImGui::DragInt(label, &v, 0, NULL, NULL, format);
}

inline void ImGui::IntBoxHalf(const char* label, int v[2], const char* format) {
  ImGui::DragInt2(label, v, 0, NULL, NULL, format);
}
