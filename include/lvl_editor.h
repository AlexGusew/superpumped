#pragma once

#include "raylib.h"
#include <vector>

enum class TargetType { A, B };

typedef struct {
  // Define anchors
  Vector2 anchor01; // ANCHOR ID:1

  // Define controls variables
  bool panelActive; // WindowBox: panel
  bool tsPrimaryEditMode;
  char tsPrimaryText[128]; // TextBox: tsPrimary
  bool tsSecondaryEditMode;
  char tsSecondaryText[128]; // TextBox: tsSecondary
  bool snapChecked;          // CheckBoxEx: snap
  bool bpmEditMode;
  int bpmValue; // ValueBOx: bpm
  bool currentBPMEditMode;
  int currentBPMValue; // ValueBOx: currentBPM
  bool currentTimeEditMode;
  int currentTimeValue; // ValueBOx: currentTime
  bool snapFrequencyEditMode;
  char snapFrequencyText[128]; // TextBox: snapFrequency

  // Define rectangles
  Rectangle layoutRecs[14];
  Rectangle splineWindow;
  Rectangle timelineWindow;

  Rectangle splineLayoutRecs[10];
  char startTimeText[32];
  char amountText[32];
  char durationText[32];
  bool startTimeEditMode = false;
  bool amountEditMode = false;
  bool durationEditMode = false;
  bool splinePanelActive = true;

  // Custom state variables (depend on development software)
  // NOTE: This variables should be added manually if required
} GuiEditorLayoutState;

typedef struct {
  std::vector<Vector2> points;
  TargetType type;
  float startTime;
  int amount;
  float duration;
  float startBit;
  float durationBit;
} Spline;

class LvlEditor {
private:
  std::tuple<int, int> FindNearestPoint(Vector2 target, float maxDist);
  Vector2 lastMousePos;
  bool wasPanning;

public:
  float curTime = 0.0f;
  bool pause = true;
  bool timelineHover;
  Vector2 mousePos;
  Vector2 mouseWorldPos;
  GuiEditorLayoutState gui;
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
};
