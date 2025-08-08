#pragma once

#include "game_object.h"
#include "raylib.h"

class Player : public GameObject {
public:
  float forceMult;
  float epsilon;
  float maxVelocity;
  float dashMagnitude;
  Vector2 velocity;
  Vector2 dashVelocity;
  Vector2 movementVelocity;

  Player();

  void Update() override;
  void HandleUserInput();
  void Draw() override;
  void DrawGizmos() override;
  void HandleDash();
};
