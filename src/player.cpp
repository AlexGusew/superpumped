#include "player.h"
#include "config.h"
#include "constants.h"
#include <algorithm>
#include <cmath>

Player::Player() {
  int h = GetScreenHeight();
  int w = GetScreenWidth();
  size = {GRID_SIZE, GRID_SIZE};
  translation = {w / 2.0f - size.x / 2, h / 2.0f - size.y / 2};
  forceMult = 500.0f;
  velocity = {0.0f, 0.0f};
  epsilon = 0.01f;
}

void Player::Update() {
  Vector2 movement = HandleUserInput();
  translation = movement;
}

Vector2 Player::HandleUserInput() {
  int h = GetScreenHeight();
  int w = GetScreenWidth();
  float deltaTime = GetFrameTime();

  if (IsKeyDown(KEY_W))
    velocity.y -= 1;
  if (IsKeyDown(KEY_S))
    velocity.y += 1;
  if (IsKeyDown(KEY_A))
    velocity.x -= 1;
  if (IsKeyDown(KEY_D))
    velocity.x += 1;

  velocity.x = std::clamp(velocity.x, -1.0f, 1.0f);
  velocity.y = std::clamp(velocity.y, -1.0f, 1.0f);

  // normalize force if it exceeds 1
  float mag = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
  if (mag > 1) {
    velocity.x = velocity.x / mag;
    velocity.y = velocity.y / mag;
  }

  velocity.x *= 0.9f;
  velocity.y *= 0.9f;

  if (std::abs(velocity.x) < epsilon)
    velocity.x = 0;
  if (std::abs(velocity.y) < epsilon)
    velocity.y = 0;

  return {std::clamp(translation.x + velocity.x * forceMult * deltaTime, 0.0f,
                     static_cast<float>(w - size.x)),
          std::clamp(translation.y + velocity.y * forceMult * deltaTime, 0.0f,
                     static_cast<float>(h - size.y))};
}

void Player::Draw() {
  DrawRectangle(static_cast<int>(translation.x),
                static_cast<int>(translation.y), static_cast<int>(size.x),
                static_cast<int>(size.y), GREEN);
  if (Config::Get().gizmosEnabled) {
    DrawGizmos();
  }
}

void Player::DrawGizmos() {
  float playerVelocityLen = 100;
  DrawLine(static_cast<int>(translation.x + size.x / 2),
           static_cast<int>(translation.y + size.y / 2),
           static_cast<int>(translation.x + size.x / 2 +
                            velocity.x * playerVelocityLen),
           static_cast<int>(translation.y + size.y / 2 +
                            velocity.y * playerVelocityLen),
           RED);
}
