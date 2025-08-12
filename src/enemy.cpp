#include "enemy.h"
#include "collision_solver.h"
#include "config.h"
#include "constants.h"
#include "raylib.h"
#include "shape_drawer.h"
#include "utils.h"
#include <iostream>

Enemy::Enemy(Vector2 _controlingPoint, float _controllingRange)
    : GameObject(_controlingPoint, {GRID_SIZE * 2, GRID_SIZE * 2},
                 new OrientedRectangle(_controlingPoint,
                                       {GRID_SIZE * 2, GRID_SIZE * 2})),
      controllingRange(_controllingRange), following(false),
      triggerCollider(_controlingPoint, _controllingRange),
      controllingPivot(_controlingPoint) {};

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
    following = true;
  }
  std::cout << distToPivot << " " << distToPlayer << std::endl;

  if (following && std::abs(distToPivot) < 700) {
    Vector2 target = player.translation;
    Vector2 dir = Utils::subtract(translation, target);
    dir = Utils::normalize(dir);
    dir = Utils::multiply(dir, -70 * GetFrameTime());
    translation = Utils::add(translation, dir);
    triggerCollider.center = translation;
  }
}
