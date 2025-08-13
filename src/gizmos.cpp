#include "gizmos.h"
#include "config.h"
#include "constants.h"

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
}

void Gizmos::DrawUI() {
  if (!Config::Get().gizmosUIEnabled) {
    return;
  }

  if (GuiButton({10, 80, 100, 20}, panelOpen ? "Hide Gizmos" : "Show Gizmos")) {
    panelOpen = !panelOpen;
  }

  if (panelOpen) {
    bool gizmosEnabled = Config::Get().gizmosEnabled;
    if (GuiCheckBox({10, 110, 20, 20}, "Draw gizmos", &gizmosEnabled)) {
      Config::Get().gizmosEnabled = gizmosEnabled;
    }
  }
}
