#include "shape_drawer.h"

void ShapeDrawer::DrawCircleOutline(const Circle *circle) {
  DrawCircleLinesV(circle->center, circle->radius, WHITE);
}

void ShapeDrawer::DrawRectangleOutline(const OrientedRectangle *rect) {
  auto vertices = rect->getVertices();

  // Draw the four edges
  for (int i = 0; i < 4; i++) {
    Vector2 start = vertices[i];
    Vector2 end = vertices[(i + 1) % 4]; // Wrap around to first vertex
    DrawLineV(start, end, WHITE);
  }
}

void ShapeDrawer::DrawShape(const Shape *shape = nullptr) {
  if (shape == nullptr)
    return;

  if (shape->getType() == Shape::CIRCLE) {
    DrawCircleOutline(static_cast<const Circle *>(shape));
  } else if (shape->getType() == Shape::ORIENTED_RECTANGLE) {
    DrawRectangleOutline(static_cast<const OrientedRectangle *>(shape));
  }
}
