#include "player.h"
#include "config.h"
#include "constants.h"
#include "raygui.h"
#include "raylib.h"
#include <algorithm>
#include <cmath>

Player::Player() {
  int h = GetScreenHeight();
  int w = GetScreenWidth();
  size = {GRID_SIZE, GRID_SIZE};
  translation = {w / 2.0f - size.x / 2, h / 2.0f - size.y / 2};
  forceMult = 50.0f;
  dashMagnitude = 10;
  maxVelocity = 10;
  velocity = {0.0f, 0.0f};
  dashVelocity = {0.0f, 0.0f};
  movementVelocity = {0.0f, 0.0f};
  epsilon = 0.01f;
}

void Player::Update() {
  float deltaTime = GetFrameTime();
  int h = GetScreenHeight();
  int w = GetScreenWidth();

  HandleUserInput();
  HandleDash();

  velocity.x = movementVelocity.x + dashVelocity.x;
  velocity.y = movementVelocity.y + dashVelocity.y;

  float dx = velocity.x * forceMult * deltaTime;
  float dy = velocity.y * forceMult * deltaTime;

  translation.x =
      std::clamp(translation.x + dx, 0.0f, static_cast<float>(w - size.x));
  translation.y =
      std::clamp(translation.y + dy, 0.0f, static_cast<float>(h - size.y));
}

void Player::HandleDash() {
  if (IsKeyPressed(KEY_SPACE)) {
    Vector2 unit = {0};
    if (IsKeyDown(KEY_W))
      unit.y -= 1;
    if (IsKeyDown(KEY_S))
      unit.y += 1;
    if (IsKeyDown(KEY_A))
      unit.x -= 1;
    if (IsKeyDown(KEY_D))
      unit.x += 1;
    if (!unit.x && !unit.y) {
      unit.x = 1;
    }
    float mag = std::sqrt(unit.x * unit.x + unit.y * unit.y);
    unit.x /= mag;
    unit.y /= mag;

    unit.x *= dashMagnitude;
    unit.y *= dashMagnitude;

    dashVelocity.x += unit.x;
    dashVelocity.y += unit.y;
  }

  dashVelocity.x *= 0.9f;
  dashVelocity.y *= 0.9f;
}

void Player::HandleUserInput() {
  if (IsKeyDown(KEY_W))
    movementVelocity.y -= 1;
  if (IsKeyDown(KEY_S))
    movementVelocity.y += 1;
  if (IsKeyDown(KEY_A))
    movementVelocity.x -= 1;
  if (IsKeyDown(KEY_D))
    movementVelocity.x += 1;

  movementVelocity.x =
      std::clamp(movementVelocity.x, -maxVelocity, maxVelocity);
  movementVelocity.y =
      std::clamp(movementVelocity.y, -maxVelocity, maxVelocity);

  // normalize force if it exceeds maxVelocity
  float mag = std::sqrt(movementVelocity.x * movementVelocity.x +
                        movementVelocity.y * movementVelocity.y);
  if (mag > maxVelocity) {
    movementVelocity.x = movementVelocity.x * maxVelocity / mag;
    movementVelocity.y = movementVelocity.y * maxVelocity / mag;
  }

  movementVelocity.x *= 0.9f;
  movementVelocity.y *= 0.9f;

  if (std::abs(movementVelocity.x) < epsilon)
    movementVelocity.x = 0;
  if (std::abs(movementVelocity.y) < epsilon)
    movementVelocity.y = 0;
}

void Player::Draw() {
  DrawRectangle(static_cast<int>(translation.x),
                static_cast<int>(translation.y), static_cast<int>(size.x),
                static_cast<int>(size.y), GREEN);
  if (Config::Get().gizmosUIEnabled) {
    DrawGizmos();
  }
}

void Player::DrawGizmos() {
  GuiLabel((Rectangle){10, 200, 100, 30},
           TextFormat("Velocity: %.1f x %.1f", velocity.x, velocity.y));
  float playerVelocityLen = 10;
  DrawLine(static_cast<int>(translation.x + size.x / 2),
           static_cast<int>(translation.y + size.y / 2),
           static_cast<int>(translation.x + size.x / 2 +
                            movementVelocity.x * playerVelocityLen),
           static_cast<int>(translation.y + size.y / 2 +
                            movementVelocity.y * playerVelocityLen),
           RED);
  DrawLine(static_cast<int>(translation.x + size.x / 2),
           static_cast<int>(translation.y + size.y / 2),
           static_cast<int>(translation.x + size.x / 2 +
                            dashVelocity.x * playerVelocityLen),
           static_cast<int>(translation.y + size.y / 2 +
                            dashVelocity.y * playerVelocityLen),
           BLUE);
}
