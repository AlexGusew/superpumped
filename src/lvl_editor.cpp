#include "lvl_editor.h"
#include "collision_solver.h"
#include "config.h"
#include "game_manager.h"
#include "raygui.h"
#include "raylib.h"
#include "utils.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <iterator>
#include <ostream>
#include <tuple>
#include <vector>

void LvlEditor::Init() {
  gui = {0};
  float w = GetScreenWidth();
  float h = GetScreenHeight();

  // Init anchors
  gui.anchor01 = (Vector2){w - 16.0f, 16.0f}; // ANCHOR ID:1

  // Initilize controls variables
  gui.panelActive = true; // WindowBox: panel
  gui.tsPrimaryEditMode = false;
  strcpy(gui.tsPrimaryText, ""); // TextBox: tsPrimary
  gui.tsSecondaryEditMode = false;
  strcpy(gui.tsSecondaryText, ""); // TextBox: tsSecondary
  gui.snapChecked = false;         // CheckBoxEx: snap
  gui.bpmEditMode = false;
  gui.bpmValue = 0; // ValueBOx: bpm
  gui.currentBPMEditMode = false;
  gui.currentBPMValue = 0; // ValueBOx: currentBPM
  gui.currentTimeEditMode = false;
  gui.currentTimeValue = 0; // ValueBOx: currentTime
  gui.snapFrequencyEditMode = false;
  strcpy(gui.snapFrequencyText, ""); // TextBox: snapFrequency

  // Init controls rectangles
  gui.layoutRecs[0] = (Rectangle){gui.anchor01.x + -160, gui.anchor01.y + 8,
                                  152, 296}; // WindowBox: panel
  gui.layoutRecs[1] = (Rectangle){gui.anchor01.x + -144, gui.anchor01.y + 40,
                                  120, 24}; // Button: pauseButton
  gui.layoutRecs[2] = (Rectangle){gui.anchor01.x + -144, gui.anchor01.y + 88,
                                  48, 24}; // TextBox: tsPrimary
  gui.layoutRecs[3] = (Rectangle){gui.anchor01.x + -88, gui.anchor01.y + 88, 12,
                                  24}; // Label: Label004
  gui.layoutRecs[4] = (Rectangle){gui.anchor01.x + -72, gui.anchor01.y + 88, 48,
                                  24}; // TextBox: tsSecondary
  gui.layoutRecs[5] = (Rectangle){gui.anchor01.x + -144, gui.anchor01.y + 64,
                                  120, 24}; // LabelButton: timeSignatureLabel
  gui.layoutRecs[6] = (Rectangle){gui.anchor01.x + -144, gui.anchor01.y + 120,
                                  24, 24}; // CheckBoxEx: snap
  gui.layoutRecs[7] = (Rectangle){gui.anchor01.x + -144, gui.anchor01.y + 144,
                                  120, 24}; // LabelButton: gui
  gui.layoutRecs[8] = (Rectangle){gui.anchor01.x + -144, gui.anchor01.y + 168,
                                  120, 24}; // ValueBOx: bpm
  gui.layoutRecs[9] = (Rectangle){gui.anchor01.x + -144, gui.anchor01.y + 192,
                                  120, 24}; // LabelButton: LabelButton009
  gui.layoutRecs[10] = (Rectangle){gui.anchor01.x + -144, gui.anchor01.y + 216,
                                   120, 24}; // ValueBOx: currentBPM
  gui.layoutRecs[11] = (Rectangle){gui.anchor01.x + -144, gui.anchor01.y + 240,
                                   120, 24}; // LabelButton: LabelButton011
  gui.layoutRecs[12] = (Rectangle){gui.anchor01.x + -144, gui.anchor01.y + 264,
                                   120, 24}; // ValueBOx: currentTime
  gui.layoutRecs[13] = (Rectangle){gui.anchor01.x + -48, gui.anchor01.y + 120,
                                   24, 24}; // TextBox: snapFrequency
  Vector2 splineAnchor = {gui.anchor01.x - 160, gui.layoutRecs[0].y +
                                                    gui.layoutRecs[0].height +
                                                    gui.anchor01.y + 8};
  gui.splineWindow = {splineAnchor.x, splineAnchor.y, 144, 300};
  gui.timelineWindow = {0, h - 115, w, 115};

  gui.splineLayoutRecs[0] = {splineAnchor.x, splineAnchor.y, 144,
                             300}; // Window
  gui.splineLayoutRecs[1] = {splineAnchor.x + 8, splineAnchor.y + 24, 60,
                             20}; // "Start time" label
  gui.splineLayoutRecs[2] = {splineAnchor.x + 8, splineAnchor.y + 46, 64,
                             20}; // Start time textbox
  gui.splineLayoutRecs[3] = {splineAnchor.x + 76, splineAnchor.y + 46, 60,
                             20}; // Start time value label
  gui.splineLayoutRecs[4] = {splineAnchor.x + 8, splineAnchor.y + 76, 60,
                             20}; // "Amount" label
  gui.splineLayoutRecs[5] = {splineAnchor.x + 8, splineAnchor.y + 98, 64,
                             20}; // Amount textbox
  gui.splineLayoutRecs[6] = {splineAnchor.x + 76, splineAnchor.y + 98, 60,
                             20}; // Amount value label
  gui.splineLayoutRecs[7] = {splineAnchor.x + 8, splineAnchor.y + 128, 60,
                             20}; // "Duration" label
  gui.splineLayoutRecs[8] = {splineAnchor.x + 8, splineAnchor.y + 150, 64,
                             20}; // Duration textbox
  gui.splineLayoutRecs[9] = {splineAnchor.x + 76, splineAnchor.y + 150, 60,
                             20}; // Duration value label
}
// LabelButton: timeSignatureLabel logic
static void TimeSignatureLabel() {
  // TODO: Implement control logic
}
// LabelButton: LabelButton007 logic
static void LabelButton007() {
  // TODO: Implement control logic
}
// LabelButton: LabelButton009 logic
static void LabelButton009() {
  // TODO: Implement control logic
}
// LabelButton: LabelButton011 logic
static void LabelButton011() {
  // TODO: Implement control logic
}

void LvlEditor::DrawUI() {
  Config &config = Config::Get();
  GameManager &gm = config.gameManager;

  float w = GetScreenWidth();
  float h = GetScreenHeight();

  float trackTotalTime = GetMusicTimeLength(gm.track);
  float totalBeats = trackTotalTime / gm.timePerBit;
  float eachBeatLen = w / totalBeats;

  DrawRectangle(0, h - 90, w, 90, {25, 25, 25, 255});
  DrawRectangle(0, h - 90, w, 20, {0, 30, 55, 255});
  for (size_t i = 0; i < splines.size(); i++) {
    Spline &spline = splines[i];
    float x = spline.startTime / trackTotalTime * w;
    float width = spline.duration / trackTotalTime * w;
    float pointW = width / spline.amount;
    DrawRectangle(x, h - 90, width, 20,
                  ColorAlpha(GREEN, i == curSpline ? 0.6 : 0.3));
    for (size_t j = 0; j < spline.amount; j++) {
      DrawLine(x + pointW * j, h - 90, x + pointW * j, h - 90 + 20,
               ColorAlpha(GREEN, 0.4));
    }
  }

  for (size_t i = 0; i < totalBeats; i++) {
    float x = i * eachBeatLen;
    if (i % 4 == 0) {
      const char *text = TextFormat("%d", i);
      float textLen = MeasureText(text, 12);
      DrawText(text, x - textLen / 2, h - 115, 12, GRAY);
      DrawLineV({x, h - 100}, {x, h - 90}, GRAY);
    } else {
      DrawLineV({x, h - 95}, {x, h - 90}, GRAY);
    }
  }

  float arrowX = w * curTime / trackTotalTime;
  DrawLine(arrowX, h - 95, arrowX, h, {255, 255, 255, 120});
  if (timelineHover) {
    DrawLine(mousePos.x, h - 95, mousePos.x, h, {255, 255, 255, 90});
  }
  DrawTriangle({arrowX, h - 90}, {arrowX + 5, h - 100}, {arrowX - 5, h - 100},
               GREEN);

  // Draw controls
  if (gui.panelActive) {
    gui.panelActive = !GuiWindowBox(gui.layoutRecs[0], "Editor Debug");
    if (GuiButton(gui.layoutRecs[1], pause ? "Play" : "Pause"))
      LvlEditor::OnPause();
    if (GuiTextBox(gui.layoutRecs[2], gui.tsPrimaryText, 128,
                   gui.tsPrimaryEditMode))
      gui.tsPrimaryEditMode = !gui.tsPrimaryEditMode;
    GuiLabel(gui.layoutRecs[3], "/");
    if (GuiTextBox(gui.layoutRecs[4], gui.tsSecondaryText, 128,
                   gui.tsSecondaryEditMode))
      gui.tsSecondaryEditMode = !gui.tsSecondaryEditMode;
    if (GuiLabelButton(gui.layoutRecs[5], "Time signature"))
      TimeSignatureLabel();
    GuiCheckBox(gui.layoutRecs[6], " Snap: 1 / ", &gui.snapChecked);
    if (GuiLabelButton(gui.layoutRecs[7], "BPM"))
      LabelButton007();
    if (GuiValueBox(gui.layoutRecs[8], NULL, &gui.bpmValue, 0, 100,
                    gui.bpmEditMode))
      gui.bpmEditMode = !gui.bpmEditMode;
    if (GuiLabelButton(gui.layoutRecs[9], "Current BPM"))
      LabelButton009();
    if (GuiValueBox(gui.layoutRecs[10], NULL, &gui.currentBPMValue, 0, 100,
                    gui.currentBPMEditMode))
      gui.currentBPMEditMode = !gui.currentBPMEditMode;
    if (GuiLabelButton(gui.layoutRecs[11], "Current sec"))
      LabelButton011();
    if (GuiValueBox(gui.layoutRecs[12], NULL, &gui.currentTimeValue, 0, 100,
                    gui.currentTimeEditMode))
      gui.currentTimeEditMode = !gui.currentTimeEditMode;
    if (GuiTextBox(gui.layoutRecs[13], gui.snapFrequencyText, 128,
                   gui.snapFrequencyEditMode))
      gui.snapFrequencyEditMode = !gui.snapFrequencyEditMode;
  }

  if (curSpline != -1) {
    Spline &spline = splines[curSpline];
    Vector2 splineAnchor = {gui.anchor01.x - 160, gui.layoutRecs[0].y +
                                                      gui.layoutRecs[0].height +
                                                      gui.anchor01.y + 8};
    GuiWindowBox({splineAnchor.x, splineAnchor.y, 144, 300}, "Spline Debug");
    splineAnchor.y += 24;

    GuiLabel(gui.splineLayoutRecs[1], "Start time");
    if (GuiTextBox(gui.splineLayoutRecs[2], gui.startTimeText, 32,
                   gui.startTimeEditMode)) {
      gui.startTimeEditMode = !gui.startTimeEditMode;
      if (!gui.startTimeEditMode) {
        spline.startBit = atof(gui.startTimeText);
        if (spline.startBit < 0.0f)
          spline.startBit = 0.0f;
        spline.startTime = spline.startBit * gm.timePerBit;
        sprintf(gui.startTimeText, "%.2f", spline.startBit);
      }
    }
    GuiLabel(gui.splineLayoutRecs[3], TextFormat("%.2f", spline.startBit));

    // Amount
    GuiLabel(gui.splineLayoutRecs[4], "Amount");
    if (GuiTextBox(gui.splineLayoutRecs[5], gui.amountText, 32,
                   gui.amountEditMode)) {
      gui.amountEditMode = !gui.amountEditMode;
      if (!gui.amountEditMode) {
        spline.amount = atoi(gui.amountText);
        if (spline.amount < 1)
          spline.amount = 1;
        if (spline.amount > 100)
          spline.amount = 100;
        sprintf(gui.amountText, "%d", spline.amount);
      }
    }
    GuiLabel(gui.splineLayoutRecs[6], TextFormat("%d", spline.amount));

    // Duration
    GuiLabel(gui.splineLayoutRecs[7], "Duration");
    if (GuiTextBox(gui.splineLayoutRecs[8], gui.durationText, 32,
                   gui.durationEditMode)) {
      gui.durationEditMode = !gui.durationEditMode;
      if (!gui.durationEditMode) {
        spline.durationBit = atof(gui.durationText);
        if (spline.durationBit < 0.1f)
          spline.durationBit = 0.1f;
        spline.duration = spline.durationBit * gm.timePerBit;
        sprintf(gui.durationText, "%.2f", spline.durationBit);
      }
    }
    GuiLabel(gui.splineLayoutRecs[9], TextFormat("%.2f", spline.durationBit));
  }
}

void LvlEditor::OnPause() {
  Config &config = Config::Get();
  Music &track = Config::Get().gameManager.track;

  pause = !pause;

  if (pause) {
    StopMusicStream(track);
    curTime -= GetFrameTime();
  } else {
    config.gameManager.SetMusicTime(curTime);
    PlayMusicStream(track);
  }
}

std::tuple<int, int> LvlEditor::FindNearestPoint(Vector2 target,
                                                 float maxDist) {
  for (size_t i = 0; i < splines.size(); i++) {
    std::vector<Vector2> &points = splines[i].points;
    for (size_t j = 0; j < points.size(); j++) {
      if (Utils::Magnitude(Utils::Subtract(points[j], target)) < maxDist) {
        return std::make_tuple(static_cast<int>(i), static_cast<int>(j));
      }
    }
  }
  return std::make_tuple(-1, -1);
}

void LvlEditor::Update() {
  Music &track = Config::Get().gameManager.track;
  Camera2D &camera = Config::Get().mainCamera;
  GameManager &gm = Config::Get().gameManager;

  float w = GetScreenWidth();
  float h = GetScreenHeight();
  float trackTotalTime = GetMusicTimeLength(track);

  if (!pause) {
    curTime += GetFrameTime();
    if (curTime > trackTotalTime) {
      curTime = 0.0f;
      OnPause();
    }
  }

  if (IsKeyPressed(KEY_SPACE)) {
    OnPause();
  }

  mousePos = GetMousePosition();
  mouseWorldPos = GetScreenToWorld2D(mousePos, camera);
  Rectangle timelineCollider = {0, h - 115, w, 25};
  timelineHover = mousePos.x > timelineCollider.x &&
                  mousePos.x < timelineCollider.x + timelineCollider.width &&
                  mousePos.y > timelineCollider.y &&
                  mousePos.y < timelineCollider.y + timelineCollider.height;
  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    if (timelineHover) {
      float snap = 1.0f / 1.0f;
      float snapTime = Config::Get().gameManager.timePerBit * snap;
      curTime = mousePos.x / w * trackTotalTime + snapTime / 2;
      curTime -= std::fmod(curTime, snapTime);
      gm.SetMusicTime(curTime);
    }
  }

  if (IsKeyPressed(KEY_R)) {
    curTime = 0.0f;
    gm.SetMusicTime(0.0f);
  }

  if (curSpline != -1 && IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_D)) {
    Spline newSpline = Spline{splines[curSpline]};
    for (Vector2 &point : newSpline.points) {
      point = Utils::Add(point, {50, 0});
    }
    splines.push_back(newSpline);
  }

  UpdateSplines();
  UpdateCamera();

  UpdateMusicStream(track);
}

void LvlEditor::UpdateSplines() {
  GameManager &gm = Config::Get().gameManager;

  bool leftPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
  bool rightPressed = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
  bool leftReleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
  bool rightReleased = IsMouseButtonReleased(MOUSE_BUTTON_RIGHT);
  bool leftDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
  bool anyPressed = leftPressed || rightPressed;
  bool anyReleased = leftReleased || rightReleased;

  // Fix double-click detection
  double currentTime = GetTime();
  bool isDoubleClick = false;
  if (anyPressed) {
    isDoubleClick = (currentTime - lastClickTime) < 0.2;
    lastClickTime = currentTime;
  }

  auto [splineIdx, pointIdx] = FindNearestPoint(mouseWorldPos, 30.0f);
  bool pointNotFound = splineIdx == -1;
  bool isFirstPoint = !pointNotFound && pointIdx == 0;

  // Handle mouse press events (including double-click actions)
  if (anyPressed) {
    if (isDoubleClick) {
      if (leftPressed && pointNotFound) {
        // Create new spline - now with shared point structure
        std::vector<Vector2> newPoints;
        Vector2 point = mouseWorldPos;
        // First spline: p1, a1, a2, p2
        for (int i = 0; i < 4; i++) {
          newPoints.push_back(point);
          point = Utils::Add(point, Utils::Multiply({1, 0}, newDistMult));
        }
        Spline newSpline = {.points = newPoints,
                            .type = TargetType::A,
                            .startTime = curTime,
                            .amount = 4,
                            .duration = 4 * gm.timePerBit,
                            .startBit =
                                static_cast<float>(curTime / gm.timePerBit),
                            .durationBit = 4};
        splines.push_back(newSpline);
      } else if (leftPressed && !pointNotFound) {
        // Extend spline
        std::vector<Vector2> &points = splines[splineIdx].points;
        bool isLastPoint = pointIdx == points.size() - 1;

        if (isFirstPoint) {
          // Add points to beginning: new_p, new_a1, new_a2, [existing_p1]
          if (points.size() >= 2) {
            Vector2 direction = Utils::Multiply(
                Utils::Normalize(Utils::Subtract(points[0], points[1])),
                newDistMult);
            Vector2 cur = points[0];
            // Insert 3 new points before the first point
            for (int i = 0; i < 3; i++) {
              cur = Utils::Add(cur, direction);
              points.insert(points.begin(), cur);
            }
          }
        } else if (isLastPoint) {
          // Add points to end: [existing_pN], new_a1, new_a2, new_p
          if (points.size() >= 2) {
            Vector2 direction =
                Utils::Multiply(Utils::Normalize(Utils::Subtract(
                                    points.back(), points[points.size() - 2])),
                                newDistMult);
            Vector2 cur = points.back();
            // Add 3 new points after the last point
            for (int i = 0; i < 3; i++) {
              cur = Utils::Add(cur, direction);
              points.push_back(cur);
            }
          }
        }
      } else if (rightPressed && !pointNotFound) {
        std::cout << "HI" << std::endl;
        // Remove points from spline
        std::vector<Vector2> &points = splines[splineIdx].points;
        bool isLastPoint = pointIdx == points.size() - 1;

        if (isFirstPoint && points.size() > 4) {
          // Remove first segment: remove first 3 points
          points.erase(points.begin(), points.begin() + 3);
        } else if (isLastPoint && points.size() > 4) {
          // Remove last segment: remove last 3 points
          points.erase(points.end() - 3, points.end());
        } else if (!isFirstPoint && !isLastPoint && points.size() > 4) {
          // Remove middle segment
          // Find which segment this point belongs to
          int segmentStart = (pointIdx / 3) * 3;
          if (segmentStart + 3 < points.size()) {
            points.erase(points.begin() + segmentStart,
                         points.begin() + segmentStart + 3);
          }
        }

        // If spline becomes too small, remove it entirely
        if (points.size() < 4) {
          splines.erase(splines.begin() + splineIdx);
          curP = -1;
          curSpline = -1;
        }
      }
    } else {
      // Single click - start dragging if we clicked on a point
      if (leftPressed && !pointNotFound) {
        curP = pointIdx;
        curSpline = splineIdx;
        Spline &spline = splines[splineIdx];
        sprintf(gui.startTimeText, "%.2f", spline.startBit);
        sprintf(gui.amountText, "%d", spline.amount);
        sprintf(gui.durationText, "%.2f", spline.durationBit);
      }
    }
  }

  // Handle mouse release events
  if (anyReleased && !isDoubleClick &&
      !CollisionSolver::pointRectCollision(gui.splineWindow, mousePos) &&
      !CollisionSolver::pointRectCollision(gui.layoutRecs[0], mousePos)) {
    if (pointNotFound) {
      curP = -1;
      curSpline = -1;
    } else {
      curP = -1;
    }
  }
  // Handle dragging
  if (leftDown && curP != -1 && curSpline != -1 && curSpline < splines.size() &&
      curP < splines[curSpline].points.size()) {
    std::vector<Vector2> &points = splines[curSpline].points;
    Vector2 oldPos = points[curP];
    Vector2 newPos = mouseWorldPos;
    Vector2 delta = Utils::Subtract(newPos, oldPos);
    if (IsKeyDown(KEY_LEFT_ALT)) {
      for (Vector2 &point : points) {
        point = Utils::Add(point, delta);
      }
    } else {
      points[curP] = newPos;
      if (curP % 3 == 0) {
        if (curP > 0) {
          points[curP - 1] = Utils::Add(points[curP - 1], delta);
        }
        if (curP < points.size() - 1) {
          points[curP + 1] = Utils::Add(points[curP + 1], delta);
        }
      }
    }
  }
}

void LvlEditor::UpdateCamera() {
  Camera2D &camera = Config::Get().mainCamera;
  // Handle camera zoom with scroll wheel
  float wheel = GetMouseWheelMove();
  if (CollisionSolver::pointRectCollision(gui.splineWindow, mousePos) ||
      CollisionSolver::pointRectCollision(gui.timelineWindow, mousePos) ||
      CollisionSolver::pointRectCollision(gui.layoutRecs[0], mousePos) ||
      CollisionSolver::pointRectCollision(gui.layoutRecs[1], mousePos)) {
    return;
  }
  if (wheel != 0) {
    // Calculate zoom factor
    float zoomIncrement = 0.1f;
    float targetZoom = camera.zoom + (wheel * zoomIncrement);

    // Clamp zoom to reasonable limits
    targetZoom = std::clamp(targetZoom, 0.1f, 3.0f);

    // Apply zoom
    camera.zoom = targetZoom;

    // Adjust camera target to zoom towards mouse position
    Vector2 newMouseWorldPos = GetScreenToWorld2D(mousePos, camera);
    Vector2 worldPosDiff = {mouseWorldPos.x - newMouseWorldPos.x,
                            mouseWorldPos.y - newMouseWorldPos.y};
    camera.target.x += worldPosDiff.x;
    camera.target.y += worldPosDiff.y;
  }

  // Handle camera panning when left mouse is down and no points selected
  bool leftDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
  if (leftDown && curP == -1 && curSpline == -1) {
    Vector2 currentMousePos = GetMousePosition();

    if (wasPanning) {
      // Calculate mouse movement in screen space
      Vector2 mouseDelta = {lastMousePos.x - currentMousePos.x,
                            lastMousePos.y - currentMousePos.y};

      // Convert mouse movement to world space movement
      Vector2 worldDelta = {mouseDelta.x / camera.zoom,
                            mouseDelta.y / camera.zoom};

      // Apply movement to camera target
      camera.target.x += worldDelta.x;
      camera.target.y += worldDelta.y;
    }

    lastMousePos = currentMousePos;
    wasPanning = true;
  } else {
    wasPanning = false;
  }
}

void LvlEditor::Draw() {
  for (size_t i = 0; i < splines.size(); i++) {
    Spline &spline = splines[i];

    float distPerOne = (spline.points.size() - 1.0f) / 3 / (spline.amount - 1);
    if (spline.amount <= 1) {
      distPerOne = 1;
    }
    for (size_t targetIdx = 0; targetIdx < spline.amount; targetIdx++) {
      float dist = distPerOne * targetIdx;
      int pStart = 3 * (int)dist;
      float curSplineDist = std::fmod(dist, 1.0f);
      if (pStart + 3 >= spline.points.size()) {
        curSplineDist = 1;
        pStart -= 3;
      }
      Vector2 p = GetSplinePointBezierCubic(
          spline.points[pStart], spline.points[pStart + 1],
          spline.points[pStart + 2], spline.points[pStart + 3], curSplineDist);
      float startTime =
          (float)targetIdx / spline.amount * spline.duration + spline.startTime;

      DrawCircleV(p, 30, ColorAlpha(curTime < startTime ? GRAY : GREEN, 0.4));
    }

    float thick = i == curSpline ? 4 : 1;
    DrawSplineBezierCubic(spline.points.data(), spline.points.size(), thick,
                          WHITE);
    for (size_t j = 0; j + 3 < spline.points.size(); j += 3) {
      DrawCircleV(spline.points[j], 5, RED);
      DrawCircleV(spline.points[j + 3], 5, RED);
      if (i == curSpline) {
        DrawCircleV(spline.points[j + 1], 5, WHITE);
        DrawCircleV(spline.points[j + 2], 5, WHITE);
        DrawLineV(spline.points[j], spline.points[j + 1], WHITE);
        DrawLineV(spline.points[j + 2], spline.points[j + 3],
                  ColorAlpha(WHITE, 0.4f));
      }
    }
  }
}
