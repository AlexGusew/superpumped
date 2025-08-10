#include "target.h"
#include "raylib.h"
#include "shape_drawer.h"
#include "utils.h"
#include <limits>

Target::Target(const Vector2 &_translation, float _readyDelta)
    : GameObject(_translation, {32, 32},
                 new OrientedRectangle(_translation, {32, 32})),
      readyDelta(_readyDelta), ready(false), reached(false) {
  readyTime = GetTime() + readyDelta;
}

void Target::Update() {
  if (GetTime() >= readyTime) {
    ready = true;
    readyTime = std::numeric_limits<float>::max();
  }
}

void Target::Draw() {
  ShapeDrawer::DrawShape(collider_shape);
  if (reached) {
    return;
  } else if (!ready) {
    Color from = DARKPURPLE;
    Color to = DARKBLUE;
    float factor = (GetTime() - readyTime + readyDelta) / readyDelta;
    Color current = ColorLerp(from, to, factor);
    DrawRectangleV(Utils::subtract(translation, Utils::multiply(size, 0.5f)),
                   size, current);
  } else {
    DrawRectangleV(Utils::subtract(translation, Utils::multiply(size, 0.5f)),
                   size, WHITE);
  }
}
