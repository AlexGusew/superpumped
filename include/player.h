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
  int hp;
  int maxHp;
  float lastHitTime;
  float hitInvulDelta;

  Player();

  void Update() override;
  void Draw() override;
  void DrawGizmos() override;
  void Reset() override;
  void DrawUI();
  void HandleUserInput();
  void HandleDash();
  void GetHit();
};
