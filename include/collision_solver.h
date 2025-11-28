#pragma once
#include "raylib.h"
#include <vector>

class Shape {
public:
  enum Type { CIRCLE, ORIENTED_RECTANGLE };

  Vector2 center;

  Shape(Vector2 c) : center(c) {}
  virtual ~Shape() = default;
  virtual Type getType() const = 0;
  virtual Vector2 getCenter() const { return center; }
};

class Circle : public Shape {
public:
  float radius;

  Circle(Vector2 c, float r) : Shape(c), radius(r) {}

  Type getType() const override { return CIRCLE; }
};

class OrientedRectangle : public Shape {
public:
  Vector2 size;
  float rotation;

  OrientedRectangle(Vector2 c, Vector2 s, float rot = 0.0f)
      : Shape(c), size(s), rotation(rot) {}

  Type getType() const override { return ORIENTED_RECTANGLE; }

  std::vector<Vector2> getVertices() const;
  std::vector<Vector2> getNormals() const;
};

struct CollisionResult {
  bool colliding;
  Vector2 penetration;
  Vector2 normal;

  CollisionResult();
  CollisionResult(bool col, Vector2 pen, Vector2 norm);
};

class CollisionSolver {
private:
  static void projectCircle(const Circle* circle, const Vector2& axis,
                            float& min, float& max);
  static void projectRectangle(const OrientedRectangle* rect,
                               const Vector2& axis, float& min, float& max);

  static bool checkOverlap(float min1, float max1, float min2, float max2,
                           float& overlap);

public:
  static CollisionResult checkCollision(const Shape* shape1,
                                        const Shape* shape2);
  static bool pointRectCollision(const Rectangle& rectangle,
                                 const Vector2 point);

  static CollisionResult checkCollision(const Circle* c1, const Circle* c2);
  static CollisionResult checkCollision(const OrientedRectangle* r1,
                                        const OrientedRectangle* r2);
  static CollisionResult checkCollision(const Circle* circle,
                                        const OrientedRectangle* rect);
  static CollisionResult checkCollision(const OrientedRectangle* rect,
                                        const Circle* circle);
  static CollisionResult checkCollision(const OrientedRectangle* rect,
                                        const Vector2* point);

  static void resolveCollision(Vector2& vel1, Vector2& vel2,
                               const Vector2& normal, float mass1 = 1.0f,
                               float mass2 = 1.0f, float restitution = 0.8f);

  static void resolveCollisionSimple(Vector2& velocity, const Vector2& normal);
};
