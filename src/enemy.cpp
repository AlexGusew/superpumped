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
      triggerCollider(_controlingPivot, {GRID_SIZE, GRID_SIZE}),
      controllingPivot(_controlingPivot), target(_controlingPivot) {};

void Enemy::Draw() {
  DrawRectangleV(translation, size, {255, 0, 0, 120});
  if (Config::Get().gizmosUIEnabled)
    ShapeDrawer::DrawShape(&triggerCollider);
}

void Enemy::Update() {
  Player &player = Config::Get().player;
  float distToPivot =
      Utils::magnitude(Utils::subtract(translation, controllingPivot));
  float distToPlayer =
      Utils::magnitude(Utils::subtract(translation, player.translation));

  if (std::abs(distToPlayer) < 300) {
    target = player.translation;
  }

  if (distToPivot > 500) {
    target = controllingPivot;
  }

  float distToTarget = Utils::magnitude(Utils::subtract(translation, target));

  if (distToTarget <= 10) {
    return;
  }

  Vector2 dir = Utils::subtract(translation, target);
  dir = Utils::normalize(dir);
  dir = Utils::multiply(dir, -70 * GetFrameTime());
  translation = Utils::add(translation, dir);
  triggerCollider.center = translation;
  collider_shape->center = translation;
}
