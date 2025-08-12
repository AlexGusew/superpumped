#pragma once

#include "collision_solver.h"
#include "game_object.h"
#include "raylib.h"

class Enemy : public GameObject {
public:
  Vector2 controllingPivot;
  Vector2 target;
  OrientedRectangle triggerCollider;
  float controllingRange;
  bool following;
  bool returning;

  Enemy(Vector2 _controllingPivot, float _controllingRange);
  void Activate();
  void Draw() override;
  void Update() override;
};
