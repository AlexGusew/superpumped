#include "timeline.h"
#include "raylib.h"
#include <iostream>
#include <ostream>

void TrackGUI::Calculate() { pos = {0, h - 90, w, 20}; }

void TrackGUI::Render() const {
  DrawRectangle(pos.x, pos.y, pos.width, pos.height, {0, 30, 55, 255});
}

void SplineGUI::Calculate() {
  GameManager& gm = Config::Get().gameManager;

  lines.clear();

  float trackTotalTime = GetMusicTimeLength(gm.track);
  float x = spline.startTime / trackTotalTime * w;
  float width = spline.duration / trackTotalTime * w;
  float pointW = width / spline.amount;
  std::cout << spline.amount << std::endl;
  pos = {x, h - 90, width, 20};
  for (size_t j = 0; j < spline.amount; j++) {
    lines.push_back({(Vector2){x + pointW * j, h - 90},
                     (Vector2){x + pointW * j, h - 90 + 20}});
  };
}

void SplineGUI::Render() const {
  DrawRectangle(pos.x, pos.y, pos.width, pos.height,
                ColorAlpha(GREEN, spline.selected ? 0.6 : 0.3));
  for (const LineGUI& line : lines) {
    DrawLineV(line.start, line.end, GRAY);
  }
}

void BeatsGUI::Calculate() {
  GameManager& gm = config.gameManager;
  float trackTotalTime = GetMusicTimeLength(gm.track);
  float totalBeats = trackTotalTime / gm.timePerBit;
  float eachBeatLen = w / totalBeats;

  texts.clear();
  lines.clear();

  for (size_t i = 0; i < totalBeats; i++) {
    float x = i * eachBeatLen;
    if (i % 4 == 0) {
      const char* text = TextFormat("%d", i);
      float textLen = MeasureText(text, 12);
      texts.push_back({{x - textLen / 2, h - 115}, text});
      lines.push_back(
          std::make_pair((Vector2){x, h - 100}, (Vector2){x, h - 90}));
    } else {
      lines.push_back(
          std::make_pair((Vector2){x, h - 95}, (Vector2){x, h - 90}));
    }
  }
}

void BeatsGUI::Render() const {
  for (const Text& t : texts) {
    DrawText(t.value.c_str(), t.pos.x, t.pos.y, 12, GRAY);
  }
  for (const auto& [a, b] : lines) {
    DrawLineV(a, b, GRAY);
  }
}

void TimelineGUI::Calculate() {
  pos = {0, h - 90, w, 90};
  beats.Calculate();
  track.Calculate();
  for (SplineGUI& spline : splines) {
    spline.Calculate();
  }
}

void TimelineGUI::Render() const {
  DrawRectangle(pos.x, pos.y, pos.width, pos.height, {25, 25, 25, 255});
  beats.Render();
  track.Render();
  for (const SplineGUI& spline : splines) {
    spline.Render();
  }
}

void TimelineGUI::NewSplineGUI(Spline& newSpline) {
  splines.emplace_back(newSpline);       // Construct directly in vector
  SplineGUI& splineGUI = splines.back(); // Get reference to the vector element

  // Setup the splineGUI
  for (size_t j = 0; j < newSpline.amount; j++) {
    splineGUI.lines.push_back(LineGUI());
  }
}
