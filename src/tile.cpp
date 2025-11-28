#include "tile.h"
#include "config.h"
#include "raylib.h"
#include "shape_drawer.h"

using std::string;

Tile::Tile(const string& tile_id, const Vector2& _translation,
           const Vector2& _size, Shape* _colliderShape = nullptr,
           const bool& _usePhysics)
    : GameObject(_translation, _size, _colliderShape), id(tile_id) {}

void Tile::Draw(TextureDrawer& drawer) {
  drawer.Draw(id, translation);
  if (Config::Get().gizmosEnabled) {
    DrawGizmos();
  }
}

void Tile::DrawGizmos() { ShapeDrawer::DrawShape(colliderShape); }
