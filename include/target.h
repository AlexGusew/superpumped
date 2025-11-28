#pragma once
#include "game_object.h"
#include "raylib.h"

class Target : public GameObject {
private:
  float readyTime;
  float readyDelta;
  float disappearDelta;
  float initialTime;
  float chargingDelta;
  float reachedTime;
  bool isPerfect;

public:
  bool reached;
  bool ready;

  Target(const Vector2& _translation, float _readyDelta, float _disappearDelta,
         float _chargingDelta);
  void Reach();
  void Update() override;
  void Draw() override;
};
