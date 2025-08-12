#include "player.h"
#include "collision_solver.h"
#include "config.h"
#include "constants.h"
#include "game_object.h"
#include "raygui.h"
#include "raylib.h"
#include "shape_drawer.h"
#include "utils.h"
#include <algorithm>
#include <cmath>

// TODO: Make Player inherited from GameObject
Player::Player()
    : GameObject({GetScreenWidth() / 2.0f - GRID_SIZE / 2.0f,
                  GetScreenHeight() / 2.0f - GRID_SIZE / 2.0f},
                 {GRID_SIZE / 2.0f, GRID_SIZE / 2.0f},
                 new Circle({GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f},
                            GRID_SIZE / 2.0f)),
      forceMult(50.0f), epsilon(0.01f), dashMagnitude(10), maxVelocity(10),
      velocity{0.0f, 0.0f}, dashVelocity{0.0f, 0.0f},
      movementVelocity{0.0f, 0.0f}, hitInvulDelta(1), hp(3), maxHp(3) {}

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

  translation.x = translation.x + dx;
  translation.y = translation.y + dy;
  collider_shape->center = Utils::add(translation, Utils::multiply(size, 0.5f));

  for (auto tile : Config::Get().terrain.tiles) {
    if (!tile.collider_shape || !tile.use_physics)
      continue;
    CollisionResult result =
        CollisionSolver::checkCollision(collider_shape, tile.collider_shape);
    if (!result.colliding)
      continue;
    translation = Utils::add(translation, result.penetration);
    CollisionSolver::resolveCollisionSimple(velocity, result.normal);
  }

  for (auto &target : Config::Get().targets) {
    if (!target.ready)
      continue;
    CollisionResult result =
        CollisionSolver::checkCollision(collider_shape, target.collider_shape);
    if (!result.colliding)
      continue;
    target.Reach();
  }

  for (auto enemy : Config::Get().enemies) {
    if (!enemy.collider_shape || lastHitTime + hitInvulDelta > GetTime())
      continue;
    CollisionResult result =
        CollisionSolver::checkCollision(collider_shape, enemy.collider_shape);
    if (!result.colliding)
      continue;
    GetHit();
  }
}

void Player::GetHit() {
  hp -= 1;
  lastHitTime = GetTime();
  if (hp < 1) {
  }
}

void Player::Reset() {
  GameObject::Reset();
  hp = maxHp;
  movementVelocity = {0, 0};
  lastHitTime = 0;
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
  Vector2 center = Utils::add(translation, Utils::multiply(size, 0.5f));
  DrawCircle(center.x, center.y, size.x, GREEN);
  if (Config::Get().gizmosUIEnabled) {
    DrawGizmos();
  }
}

void Player::DrawUI() {
  Color hpColor = RED;
  Color missingColor = GRAY;
  Vector2 offset = {20, 20};
  Vector2 hpSize = {40, 40};

  for (int i = 0; i < maxHp; i++) {
    Color current = i <= hp ? hpColor : missingColor;
    Vector2 position = Utils::add(offset, {(hpSize.x + 20) * i, 0});
    DrawRectangleV(position, hpSize, current);
  }
}

void Player::DrawGizmos() {
  GuiLabel((Rectangle){10, 30, 100, 50},
           TextFormat("Velocity: %.1f x %.1f", velocity.x, velocity.y));
  ShapeDrawer::DrawShape(collider_shape);
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
