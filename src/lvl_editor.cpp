#include "lvl_editor.h"
#include "config.h"
#include "game_manager.h"
#include "imgui.h"
#include "raygui.h"
#include "raylib.h"
#include "rlImGui.h"
#include "timeline.h"
#include "utils.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <tuple>
#include <vector>

void LvlEditor::Init() { splines.reserve(1024); }

void LvlEditor::DrawUI() {
  Config& config = Config::Get();
  GameManager& gm = config.gameManager;

  float w = GetScreenWidth();
  float h = GetScreenHeight();

  float trackTotalTime = GetMusicTimeLength(gm.track);
  float totalBeats = trackTotalTime / gm.timePerBit;
  float eachBeatLen = w / totalBeats;

  timelineGUI.Render();

  float arrowX = w * curTime / trackTotalTime;
  DrawLine(arrowX, h - 95, arrowX, h, {255, 255, 255, 120});
  if (timelineHover) {
    DrawLine(mousePos.x, h - 95, mousePos.x, h, {255, 255, 255, 90});
  }
  DrawTriangle({arrowX, h - 90}, {arrowX + 5, h - 100}, {arrowX - 5, h - 100},
               GREEN);

  rlImGuiBegin();
  bool debugPanelOpen = true;

  if (ImGui::Begin("Editor Debug", &debugPanelOpen)) {
    if (ImGui::Button(pause ? "Play (space)" : "Pause (space)")) {
      LvlEditor::OnPause();
    }
    if (ImGui::Button("Reset (r)")) {
      LvlEditor::Restart();
    }
    ImGui::Text("Beat: %.2f", curTime / gm.timePerBit);
    ImGui::Text("Time: %.2f", curTime);
    if (curSpline != -1) {
      ImGui::SeparatorText("Spline");
      Spline& spline = splines[curSpline];
      ImGui::Text("Start time");
      ImGui::SetNextItemWidth(-1);
      if (ImGui::InputFloat("##startTime", &spline.startBit, 0.1f, 1.0f,
                            "%.2f")) {
        spline.startBit = std::max(0.0f, spline.startBit);
        spline.startTime = spline.startBit * gm.timePerBit;
      }
      ImGui::Text("Amount");
      ImGui::SetNextItemWidth(-1);
      if (ImGui::InputInt("##amount", &spline.amount, 1, 10)) {
        spline.amount = std::clamp(spline.amount, 1, 100);
      }
      ImGui::Text("Duration");
      ImGui::SetNextItemWidth(-1);
      if (ImGui::InputFloat("##duration", &spline.durationBit, 0.1f, 1.0f,
                            "%.2f")) {
        spline.durationBit = std::max(0.1f, spline.durationBit);
        spline.duration = spline.durationBit * gm.timePerBit;
      }
    }
  }
  ImGui::End();
  rlImGuiEnd();
}

void LvlEditor::OnPause() {
  Config& config = Config::Get();
  Music& track = Config::Get().gameManager.track;

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
    std::vector<Vector2>& points = splines[i].points;
    for (size_t j = 0; j < points.size(); j++) {
      if (Utils::Magnitude(Utils::Subtract(points[j], target)) < maxDist) {
        return std::make_tuple(static_cast<int>(i), static_cast<int>(j));
      }
    }
  }
  return std::make_tuple(-1, -1);
}

void LvlEditor::Restart() {
  GameManager& gm = Config::Get().gameManager;
  curTime = 0.0f;
  gm.SetMusicTime(0.0f);
  if (pause) {
    OnPause();
  }
}

void LvlEditor::Update() {
  Music& track = Config::Get().gameManager.track;
  Camera2D& camera = Config::Get().mainCamera;
  GameManager& gm = Config::Get().gameManager;

  float w = GetScreenWidth();
  float h = GetScreenHeight();
  float trackTotalTime = GetMusicTimeLength(track);
  float totalBeats = trackTotalTime / gm.timePerBit;
  float eachBeatLen = w / totalBeats;
  Rectangle timelineCollider = {0, h - 115, w, 25};

  mousePos = GetMousePosition();
  mouseWorldPos = GetScreenToWorld2D(mousePos, camera);

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

  timelineHover = mousePos.x > timelineCollider.x &&
                  mousePos.x < timelineCollider.x + timelineCollider.width &&
                  mousePos.y > timelineCollider.y &&
                  mousePos.y < timelineCollider.y + timelineCollider.height;
  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && timelineHover) {
    float snap = 1.0f / 1.0f;
    float snapTime = Config::Get().gameManager.timePerBit * snap;
    curTime = mousePos.x / w * trackTotalTime + snapTime / 2;
    curTime -= std::fmod(curTime, snapTime);
    gm.SetMusicTime(curTime);
  }

  if (IsKeyPressed(KEY_R)) {
    Restart();
  }

  if (curSpline != -1 && IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_D)) {
    DuplicateSpline(curTime, splines[curSpline]);
  }

  UpdateSplines();
  UpdateCamera();
  UpdateMusicStream(track);
  timelineGUI.Calculate();
}

void LvlEditor::DuplicateSpline(float time, Spline& referenceSpline) {
  GameManager& gm = Config::Get().gameManager;
  float w = GetScreenWidth();
  float h = GetScreenHeight();

  splines.push_back(Spline{referenceSpline});
  splines.back().startTime = time;

  for (Vector2& point : splines.back().points) {
    point = Utils::Add(point, {50, 0});
  }
  timelineGUI.NewSplineGUI(splines.back());
}

void LvlEditor::NewSpline(float time, Vector2& startPoint) {
  GameManager& gm = Config::Get().gameManager;
  std::vector<Vector2> newPoints;

  for (int i = 0; i < 4; i++) {
    newPoints.push_back(startPoint);
    startPoint = Utils::Add(startPoint, Utils::Multiply({1, 0}, newDistMult));
  }
  splines.emplace_back(
      Spline{.points = newPoints,
             .type = TargetType::A,
             .startTime = time,
             .amount = 4,
             .duration = 4 * gm.timePerBit,
             .startBit = static_cast<float>(time / gm.timePerBit),
             .durationBit = 4});
  timelineGUI.NewSplineGUI(splines.back());
}

void LvlEditor::UpdateSplines() {
  GameManager& gm = Config::Get().gameManager;

  // ImGuiIO& io = ImGui::GetIO();
  //
  // if (io.WantCaptureMouse)
  //   return;

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
        NewSpline(curTime, mouseWorldPos);
      } else if (leftPressed && !pointNotFound) {
        // Extend spline
        std::vector<Vector2>& points = splines[splineIdx].points;
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
        // Remove points from spline
        std::vector<Vector2>& points = splines[splineIdx].points;
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
        Spline& spline = splines[splineIdx];
      }
    }
  }

  // Handle mouse release events
  if (anyReleased && !isDoubleClick) {
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
    std::vector<Vector2>& points = splines[curSpline].points;
    Vector2 oldPos = points[curP];
    Vector2 newPos = mouseWorldPos;
    Vector2 delta = Utils::Subtract(newPos, oldPos);
    if (IsKeyDown(KEY_LEFT_ALT)) {
      for (Vector2& point : points) {
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
  // ImGuiIO& io = ImGui::GetIO();
  //
  // if (io.WantCaptureMouse)
  //   return;

  Camera2D& camera = Config::Get().mainCamera;
  float wheel = GetMouseWheelMove();

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
    Spline& spline = splines[i];

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
