#pragma once
#include "collision_solver.h"

class ShapeDrawer {
private:
  static void DrawCircleOutline(const Circle *circle);
  static void DrawRectangleOutline(const OrientedRectangle *rect);

public:
  static void DrawShape(const Shape *shape);
};
