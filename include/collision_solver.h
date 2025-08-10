#pragma once
#include "raylib.h"
#include <vector>

// Shape base class
class Shape {
public:
  enum Type { CIRCLE, ORIENTED_RECTANGLE };

  Vector2 center;

  Shape(Vector2 c) : center(c) {}
  virtual ~Shape() = default;
  virtual Type getType() const = 0;
  virtual Vector2 getCenter() const { return center; }
};

// Circle shape
class Circle : public Shape {
public:
  float radius;

  Circle(Vector2 c, float r) : Shape(c), radius(r) {}

  Type getType() const override { return CIRCLE; }
};

// Oriented Rectangle shape
class OrientedRectangle : public Shape {
public:
  Vector2 size;   // width, height
  float rotation; // in radians

  OrientedRectangle(Vector2 c, Vector2 s, float rot = 0.0f)
      : Shape(c), size(s), rotation(rot) {}

  Type getType() const override { return ORIENTED_RECTANGLE; }

  // Get the four corners of the rectangle
  std::vector<Vector2> getVertices() const;

  // Get the normals (perpendicular to edges)
  std::vector<Vector2> getNormals() const;
};

struct CollisionResult {
  bool colliding;
  Vector2 penetration; // How much to move first object to resolve collision
  Vector2 normal; // Collision normal (normalized direction from obj1 to obj2)

  CollisionResult();
  CollisionResult(bool col, Vector2 pen, Vector2 norm);
};

class CollisionSolver {
private:
  // Projection functions
  static void projectCircle(const Circle *circle, const Vector2 &axis,
                            float &min, float &max);
  static void projectRectangle(const OrientedRectangle *rect,
                               const Vector2 &axis, float &min, float &max);

  // Overlap checking
  static bool checkOverlap(float min1, float max1, float min2, float max2,
                           float &overlap);

public:
  // Helper functions

  // Main collision detection - works with base Shape class
  static CollisionResult checkCollision(const Shape *shape1,
                                        const Shape *shape2);

  // Specific collision detection methods
  static CollisionResult checkCollision(const Circle *c1, const Circle *c2);
  static CollisionResult checkCollision(const OrientedRectangle *r1,
                                        const OrientedRectangle *r2);
  static CollisionResult checkCollision(const Circle *circle,
                                        const OrientedRectangle *rect);
  static CollisionResult checkCollision(const OrientedRectangle *rect,
                                        const Circle *circle);

  // Collision resolution helpers
  static void resolveCollision(Vector2 &vel1, Vector2 &vel2,
                               const Vector2 &normal, float mass1 = 1.0f,
                               float mass2 = 1.0f, float restitution = 0.8f);

  // Simple collision resolution (stops movement in collision direction)
  static void resolveCollisionSimple(Vector2 &velocity, const Vector2 &normal);
};
