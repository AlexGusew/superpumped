#include "collision_solver.h"
#include "utils.h"
#include <cmath>

// OrientedRectangle implementation
std::vector<Vector2> OrientedRectangle::getVertices() const {
  std::vector<Vector2> vertices(4);
  float cos_r = cosf(rotation);
  float sin_r = sinf(rotation);
  float half_w = size.x * 0.5f;
  float half_h = size.y * 0.5f;

  // Local vertices (relative to center)
  Vector2 local_vertices[4] = {{-half_w, -half_h},
                               {half_w, -half_h},
                               {half_w, half_h},
                               {-half_w, half_h}};

  // Transform to world space
  for (int i = 0; i < 4; i++) {
    vertices[i].x =
        center.x + local_vertices[i].x * cos_r - local_vertices[i].y * sin_r;
    vertices[i].y =
        center.y + local_vertices[i].x * sin_r + local_vertices[i].y * cos_r;
  }

  return vertices;
}

std::vector<Vector2> OrientedRectangle::getNormals() const {
  std::vector<Vector2> normals(2);
  float cos_r = cosf(rotation);
  float sin_r = sinf(rotation);

  // Two perpendicular normals for the rectangle
  normals[0] = {cos_r, sin_r};  // Along width
  normals[1] = {-sin_r, cos_r}; // Along height

  return normals;
}

// CollisionResult implementation
CollisionResult::CollisionResult()
    : colliding(false), penetration({0, 0}), normal({0, 0}) {}

CollisionResult::CollisionResult(bool col, Vector2 pen, Vector2 norm)
    : colliding(col), penetration(pen), normal(norm) {}

void CollisionSolver::projectCircle(const Circle *circle, const Vector2 &axis,
                                    float &min, float &max) {
  float center_projection = Utils::dot(circle->center, axis);
  min = center_projection - circle->radius;
  max = center_projection + circle->radius;
}

void CollisionSolver::projectRectangle(const OrientedRectangle *rect,
                                       const Vector2 &axis, float &min,
                                       float &max) {
  auto vertices = rect->getVertices();
  min = max = Utils::dot(vertices[0], axis);

  for (size_t i = 1; i < vertices.size(); i++) {
    float projection = Utils::dot(vertices[i], axis);
    min = std::min(min, projection);
    max = std::max(max, projection);
  }
}

bool CollisionSolver::checkOverlap(float min1, float max1, float min2,
                                   float max2, float &overlap) {
  if (max1 < min2 || max2 < min1) {
    return false; // No overlap
  }

  overlap = std::min(max1, max2) - std::max(min1, min2);
  return true;
}

// Main collision detection using polymorphism
CollisionResult CollisionSolver::checkCollision(const Shape *shape1,
                                                const Shape *shape2) {
  if (shape1->getType() == Shape::CIRCLE &&
      shape2->getType() == Shape::CIRCLE) {
    return checkCollision(static_cast<const Circle *>(shape1),
                          static_cast<const Circle *>(shape2));
  } else if (shape1->getType() == Shape::ORIENTED_RECTANGLE &&
             shape2->getType() == Shape::ORIENTED_RECTANGLE) {
    return checkCollision(static_cast<const OrientedRectangle *>(shape1),
                          static_cast<const OrientedRectangle *>(shape2));
  } else if (shape1->getType() == Shape::CIRCLE &&
             shape2->getType() == Shape::ORIENTED_RECTANGLE) {
    return checkCollision(static_cast<const Circle *>(shape1),
                          static_cast<const OrientedRectangle *>(shape2));
  } else if (shape1->getType() == Shape::ORIENTED_RECTANGLE &&
             shape2->getType() == Shape::CIRCLE) {
    return checkCollision(static_cast<const OrientedRectangle *>(shape1),
                          static_cast<const Circle *>(shape2));
  }

  // Should never reach here with valid shapes
  return CollisionResult(false, {0, 0}, {0, 0});
}

// Circle-Circle collision using distance check
CollisionResult CollisionSolver::checkCollision(const Circle *c1,
                                                const Circle *c2) {
  Vector2 diff = Utils::subtract(c2->center, c1->center);
  float distance_sq = diff.x * diff.x + diff.y * diff.y;
  float radius_sum = c1->radius + c2->radius;

  if (distance_sq >= radius_sum * radius_sum) {
    return CollisionResult(false, {0, 0}, {0, 0});
  }

  float distance = sqrtf(distance_sq);
  float overlap = radius_sum - distance;

  if (distance > 0) {
    Vector2 normal = {diff.x / distance, diff.y / distance};
    Vector2 penetration_vec = Utils::multiply(normal, -overlap);
    return CollisionResult(true, penetration_vec, normal);
  } else {
    // Circles are at exact same position
    Vector2 normal = {1, 0}; // Default normal
    return CollisionResult(true, {-overlap, 0}, normal);
  }
}

// OrientedRectangle-OrientedRectangle collision using SAT
CollisionResult CollisionSolver::checkCollision(const OrientedRectangle *r1,
                                                const OrientedRectangle *r2) {
  std::vector<Vector2> axes;

  // Get normals from both rectangles
  auto normals1 = r1->getNormals();
  auto normals2 = r2->getNormals();

  axes.insert(axes.end(), normals1.begin(), normals1.end());
  axes.insert(axes.end(), normals2.begin(), normals2.end());

  float min_overlap = std::numeric_limits<float>::max();
  Vector2 separation_axis = {0, 0};

  for (const auto &axis : axes) {
    float min1, max1, min2, max2;
    projectRectangle(r1, axis, min1, max1);
    projectRectangle(r2, axis, min2, max2);

    float overlap;
    if (!checkOverlap(min1, max1, min2, max2, overlap)) {
      return CollisionResult(false, {0, 0}, {0, 0}); // Separating axis found
    }

    if (overlap < min_overlap) {
      min_overlap = overlap;
      separation_axis = axis;

      // Make sure separation axis points from r1 to r2
      Vector2 center_diff = Utils::subtract(r2->center, r1->center);
      if (Utils::dot(separation_axis, center_diff) < 0) {
        separation_axis = Utils::multiply(separation_axis, -1.0f);
      }
    }
  }

  Vector2 normal = Utils::normalize(separation_axis);
  Vector2 penetration_vec = Utils::multiply(normal, -min_overlap);
  return CollisionResult(true, penetration_vec, normal);
}

// Circle-OrientedRectangle collision using SAT with special handling
CollisionResult CollisionSolver::checkCollision(const Circle *circle,
                                                const OrientedRectangle *rect) {
  std::vector<Vector2> axes;

  // Add rectangle normals
  auto rect_normals = rect->getNormals();
  axes.insert(axes.end(), rect_normals.begin(), rect_normals.end());

  // Find closest vertex to circle center for potential separating axis
  auto vertices = rect->getVertices();
  float closest_dist_sq = std::numeric_limits<float>::max();
  Vector2 closest_vertex;

  for (const auto &vertex : vertices) {
    Vector2 diff = Utils::subtract(circle->center, vertex);
    float dist_sq = diff.x * diff.x + diff.y * diff.y;
    if (dist_sq < closest_dist_sq) {
      closest_dist_sq = dist_sq;
      closest_vertex = vertex;
    }
  }

  // Add axis from closest vertex to circle center
  Vector2 vertex_to_circle = Utils::subtract(circle->center, closest_vertex);
  if (vertex_to_circle.x != 0 || vertex_to_circle.y != 0) {
    axes.push_back(Utils::normalize(vertex_to_circle));
  }

  float min_overlap = std::numeric_limits<float>::max();
  Vector2 separation_axis = {0, 0};

  for (const auto &axis : axes) {
    float min_circle, max_circle, min_rect, max_rect;
    projectCircle(circle, axis, min_circle, max_circle);
    projectRectangle(rect, axis, min_rect, max_rect);

    float overlap;
    if (!checkOverlap(min_circle, max_circle, min_rect, max_rect, overlap)) {
      return CollisionResult(false, {0, 0}, {0, 0}); // Separating axis found
    }

    if (overlap < min_overlap) {
      min_overlap = overlap;
      separation_axis = axis;

      // Make sure separation axis points from circle to rect
      Vector2 center_diff = Utils::subtract(rect->center, circle->center);
      if (Utils::dot(separation_axis, center_diff) < 0) {
        separation_axis = Utils::multiply(separation_axis, -1.0f);
      }
    }
  }

  Vector2 normal = Utils::normalize(separation_axis);
  Vector2 penetration_vec = Utils::multiply(normal, -min_overlap);
  return CollisionResult(true, penetration_vec, normal);
}

// OrientedRectangle-Circle collision (reverse parameter order)
CollisionResult CollisionSolver::checkCollision(const OrientedRectangle *rect,
                                                const Circle *circle) {
  CollisionResult result = checkCollision(circle, rect);
  // Flip the penetration vector since we swapped the order
  result.penetration = Utils::multiply(result.penetration, -1.0f);
  // Also flip the normal
  result.normal = Utils::multiply(result.normal, -1.0f);
  return result;
}

// Simple collision resolution (stops movement in collision direction)
void CollisionSolver::resolveCollisionSimple(Vector2 &velocity,
                                             const Vector2 &normal) {
  // Remove velocity component in the direction of collision
  float velAlongNormal = Utils::dot(velocity, normal);
  if (velAlongNormal < 0) { // Only if moving towards the collision
    Vector2 velocityNormal = Utils::multiply(normal, velAlongNormal);
    velocity = Utils::subtract(velocity, velocityNormal);
  }
}
