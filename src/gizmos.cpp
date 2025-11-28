#include "gizmos.h"
#include "config.h"
#include "constants.h"

#include "imgui.h"
#include "raygui.h"
#include "rlImGui.h"

void Gizmos::Draw() {
  if (!Config::Get().gizmosEnabled) {
    return;
  }
  int h = GetScreenHeight() * 10;
  int w = GetScreenWidth() * 10;
  int i = -h;
  while (i < h) {
    DrawLine(-w, i, w, i, DGRAY);
    i += GRID_SIZE;
  }
  i = -w;
  while (i < w) {
    DrawLine(i, -h, i, h, DGRAY);
    i += GRID_SIZE;
  }
}

void Gizmos::DrawUI() {
  // rlImGuiBegin();

  // bool open = false;
  // if (ImGui::Begin("Debug", &open)) {
  //   ImGui::Checkbox("Show gizmos", &Config::Get().gizmosEnabled);
  // }
  // ImGui::End();

  // rlImGuiEnd();
}
