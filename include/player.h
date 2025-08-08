#pragma once

#include "game_object.h"
#include "raylib.h"

class Player : public GameObject {
public:
  float forceMult;
  float epsilon;
  Vector2 velocity;

  Player();

  void Update() override;
  Vector2 HandleUserInput();
  void Draw() override;
  void DrawGizmos() override;
};
