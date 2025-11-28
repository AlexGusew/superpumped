#include "enemy.h"
#include "collision_solver.h"
#include "config.h"
#include "constants.h"
#include "raylib.h"
#include "shape_drawer.h"
#include "utils.h"

Enemy::Enemy(Vector2 _controlingPivot, float _controllingRange)
    : GameObject(_controlingPivot, {GRID_SIZE * 2, GRID_SIZE * 2},
                 new OrientedRectangle(_controlingPivot,
                                       {GRID_SIZE * 2, GRID_SIZE * 2})),
      controllingRange(_controllingRange), following(false),
      controllingPivot(_controlingPivot), target(_controlingPivot),
      maxFromPivotDist(500), triggerDist(300), minToTargetDist(5) {};

void Enemy::Draw() {
  DrawRectangleV(Utils::Subtract(translation, Utils::Multiply(size, 0.5)), size,
                 {255, 0, 0, 120});
  if (Config::Get().gizmosEnabled) {
    ShapeDrawer::DrawShape(colliderShape);
  }
}

void Enemy::Update() {
  Player& player = Config::Get().player;
  float distToPivot =
      Utils::Magnitude(Utils::Subtract(translation, controllingPivot));
  float distToPlayer =
      Utils::Magnitude(Utils::Subtract(translation, player.translation));

  if (std::abs(distToPlayer) < triggerDist) {
    target = player.translation;
  }

  if (distToPivot > maxFromPivotDist) {
    target = controllingPivot;
  }

  float distToTarget = Utils::Magnitude(Utils::Subtract(translation, target));

  if (distToTarget <= minToTargetDist) {
    return;
  }

  Vector2 dir = Utils::Subtract(translation, target);
  dir = Utils::Normalize(dir);
  dir = Utils::Multiply(dir, -70 * GetFrameTime());
  translation = Utils::Add(translation, dir);
  colliderShape->center = translation;
}
