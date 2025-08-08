#include "gizmos.h"
#include "config.h"
#include "constants.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

void Gizmos::Update() {
  if (IsKeyPressed(KEY_F1)) {
    Config::Get().gizmosUIEnabled = !Config::Get().gizmosUIEnabled;
  }
}

void Gizmos::Draw() {
  if (!Config::Get().gizmosUIEnabled) {
    return;
  }

  if (Config::Get().gizmosEnabled) {
    int h = GetScreenHeight();
    int w = GetScreenWidth();
    int i = GRID_SIZE;
    while (i < h) {
      DrawLine(0, i, w, i, DGRAY);
      i += GRID_SIZE;
    }
    i = GRID_SIZE;
    while (i < w) {
      DrawLine(i, 0, i, h, DGRAY);
      i += GRID_SIZE;
    }
  }

  if (GuiButton({10, 10, 100, 20}, panelOpen ? "Hide Gizmos" : "Show Gizmos")) {
    panelOpen = !panelOpen;
  }

  if (panelOpen) {
    bool gizmosEnabled = Config::Get().gizmosEnabled;
    if (GuiCheckBox({20, 40, 20, 20}, "Draw gizmos", &gizmosEnabled)) {
      Config::Get().gizmosEnabled = gizmosEnabled;
    }
  }
}
