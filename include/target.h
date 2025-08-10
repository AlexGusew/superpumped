#pragma once
#include "game_object.h"
#include "raylib.h"

class Target : public GameObject {
private:
  float readyTime;
  float readyDelta;

public:
  bool reached;
  bool ready;

  Target(const Vector2 &_translation, float _readyDelta);

  void Update() override;
  void Draw() override;
};
