#include "terrain.h"
#include "collision_solver.h"
#include "constants.h"
#include "raylib.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

void Terrain::load(const std::string &_path) {
  path = _path;

  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + path);
  }

  std::string line;
  std::string token;

  positions.clear();

  while (std::getline(file, line)) {
    if (line.empty()) {
      continue;
    }

    std::vector<int> row;
    std::stringstream ss(line);

    while (std::getline(ss, token, ',')) {
      try {
        // Trim whitespace from token
        token.erase(0, token.find_first_not_of(" \t"));
        token.erase(token.find_last_not_of(" \t") + 1);

        if (!token.empty()) {
          row.push_back(std::stoi(token));
        }
      } catch (const std::invalid_argument &e) {
        throw std::runtime_error("Invalid number format in file: " + token);
      } catch (const std::out_of_range &e) {
        throw std::runtime_error("Number out of range in file: " + token);
      }
    }

    if (!row.empty()) {
      positions.push_back(row);
    }
  }

  file.close();

  if (positions.empty()) {
    throw std::runtime_error("No valid data loaded from file: " + path);
  }
}

void Terrain::generate_tiles() {
  tiles.clear();

  if (positions.empty()) {
    std::cerr << "Warning: No terrain data loaded. Call load() first."
              << std::endl;
    return;
  }

  size_t total_tiles = 0;
  for (const auto &row : positions) {
    for (int tile_type : row) {
      if (tile_type != 0) { // Assuming 0 means empty
        total_tiles++;
      }
    }
  }
  tiles.reserve(total_tiles);

  for (size_t i = 0; i < positions.size(); ++i) {
    for (size_t j = 0; j < positions[i].size(); ++j) {
      Vector2 translation = {static_cast<float>(j * GRID_SIZE),
                             static_cast<float>(i * GRID_SIZE)};
      Vector2 size = {GRID_SIZE, GRID_SIZE};
      switch (positions[i][j]) {
      case 1: {
        OrientedRectangle *collider = new OrientedRectangle(
            {
                translation.x + size.x / 2,
                translation.y + size.y / 2,
            },
            size);
        tiles.push_back(Tile("wall", translation, size, collider, true));
        break;
      }
      case 2:
        tiles.push_back(Tile("grass", translation, size, nullptr));
        break;
      case 3:
        tiles.push_back(Tile("water", translation, size, nullptr));
        break;
      default:
        break;
      }
    }
  }

  std::cout << "Generated " << tiles.size() << " tiles." << std::endl;
}

void Terrain::clear() {
  positions.clear();
  tiles.clear();
}
