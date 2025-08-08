#pragma once

#include "tile.h"
#include <string>
#include <vector>

class Terrain {
private:
  std::string path;

public:
  std::vector<std::vector<int>> positions;
  std::vector<Tile> tiles;

  Terrain() = default;

  ~Terrain() = default;

  void load(const std::string &_path);
  void generate_tiles();
  void clear();
};
