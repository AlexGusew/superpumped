#pragma once

#include "raylib.h"
#include "timeline.h"
#include <vector>

class LvlEditor {
private:
  std::tuple<int, int> FindNearestPoint(Vector2 target, float maxDist);
  Vector2 lastMousePos;
  bool wasPanning;
  void Restart();

public:
  TimelineGUI timelineGUI;
  float curTime = 0.0f;
  bool pause = true;
  bool timelineHover;
  Vector2 mousePos;
  Vector2 mouseWorldPos;
  float lastClickTime;
  float newDistMult = 40.0f;

  int curP = -1;
  int curSpline = -1;
  std::vector<Spline> splines;
  void Init();
  void Update();
  void UpdateSplines();
  void UpdateCamera();
  void Draw();
  void DrawUI();
  void OnPause();
  void NewSpline(float time, Vector2& startPoint);
  void DuplicateSpline(float time, Spline& referenceSpline);
};
