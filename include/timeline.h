#pragma once

#include "config.h"
#include "raylib.h"
#include <string>
#include <vector>

enum class TargetType { A, B };

typedef struct {
  std::vector<Vector2> points;
  TargetType type;
  float startTime;
  int amount;
  float duration;
  float startBit;
  float durationBit;
  bool selected;
} Spline;

class GUINode {
protected:
  Config& config;
  float& w;
  float& h;

public:
  GUINode() : config(Config::Get()), w(config.w), h(config.h) {}
  virtual void Calculate() = 0;
  virtual void Render() const = 0;
  virtual ~GUINode() = default;
};

class BeatsGUI : public GUINode {
  struct Text {
    Vector2 pos;
    std::string value;
  };
  std::vector<std::pair<Vector2, Vector2>> lines;
  std::vector<Text> texts;
  Rectangle pos;

public:
  void Calculate() override;
  void Render() const override;
};

class TrackGUI : public GUINode {
  Rectangle pos;

public:
  void Calculate() override;
  void Render() const override;
};

typedef struct {
  Vector2 start;
  Vector2 end;
} LineGUI;

class SplineGUI : public GUINode {
  Spline& spline;
  Rectangle pos;

public:
  std::vector<LineGUI> lines;

  void Calculate() override;
  void Render() const override;

  SplineGUI(Spline& _spline) : spline(_spline) {};
};

class TimelineGUI : public GUINode {
  Rectangle pos;
  std::vector<SplineGUI> splines;
  BeatsGUI beats;
  TrackGUI track;

public:
  TimelineGUI() { splines.reserve(1024); }

  void Calculate() override;
  void Render() const override;

  void NewSplineGUI(Spline& newSpline);
};
