#include "raylib.h"
#include "resource_dir.h" // utility header for SearchAndSetResourceDir

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;
using namespace std::string_literals;

Color DGRAY = {30, 30, 30, 255};

class Terrain {
private:
  string path;

public:
  vector<vector<int>> positions;

  Terrain(const string &_path) : path(_path), positions({}) {}

  void Load() {
    ifstream file(path);
    if (!file.is_open()) {
      throw runtime_error("Failed to open file: " + path);
    }

    string line;
    string token;

    while (getline(file, line)) {
      if (line.empty()) {
        continue;
      }

      vector<int> row;
      stringstream ss(line);

      while (getline(ss, token, ',')) {
        try {
          row.push_back(stoi(token));
        } catch (const invalid_argument &e) {
          throw runtime_error("Invalid number format in file: " + token);
        }
      }

      if (!row.empty()) {
        positions.push_back(row);
      }
    }

    file.close();
  }

  void printTerrain() const {
    cout << "terrain" << endl;
    for (const auto &row : positions) {
      for (size_t i = 0; i < row.size(); ++i) {
        cout << row[i];
        if (i < row.size() - 1)
          cout << ",";
      }
      cout << "\n";
    }
  }

  void clear() { positions.clear(); }
};

class TextureDrawer {
private:
  Texture texture;

public:
  unordered_map<string, pair<int, int>> config;
  int size;
  TextureDrawer(std::string path, unordered_map<string, pair<int, int>> _config,
                int _size)
      : config(_config), size(_size) {
    texture = LoadTexture("spritesheet.png");
  }

  void Draw(string id, Vector2 position) {
    if (config.find(id) != config.end()) {
      int row = config[id].first;
      int col = config[id].second;
      DrawTextureRec(texture,
                     {static_cast<float>(size * col),
                      static_cast<float>(size * row), static_cast<float>(size),
                      static_cast<float>(size)},
                     position, WHITE);
    }
  }

  ~TextureDrawer() { UnloadTexture(texture); }
};

int main() {
  // Tell the window to use vsync and work on high DPI displays
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);

  // Create the window and OpenGL context
  InitWindow(1280, 800, "Hello Raylib");
  SetTargetFPS(60);

  // Utility function from resource_dir.h to find the resources folder and set
  // it as the current working directory so we can load from it
  SearchAndSetResourceDir("resources");

  TextureDrawer spreadsheet("spreadsheet",
                            {
                                {"wall", {0, 1}},
                                {"grass", {0, 2}},
                                {"water", {0, 3}},
                            },
                            32);
  Terrain terrainMap("terrain.csv");
  terrainMap.Load();
  terrainMap.printTerrain();

  int h = GetScreenHeight();
  int w = GetScreenWidth();
  Vector2 playerSize = {50.0f, 50.0f};
  Vector2 playerCoords = {w / 2.0f - playerSize.x / 2,
                          h / 2.0f - playerSize.y / 2};
  float forceMult = 10.0f;
  Vector2 velocity = {0.0f, 0.0f};
  float epsilon = 0.01f;

  // game loop
  while (!WindowShouldClose()) // run the loop untill the user presses ESCAPE or
                               // presses the Close button on the window
  {
    if (IsKeyDown(KEY_W)) {
      velocity.y -= 1;
    }
    if (IsKeyDown(KEY_S)) {
      velocity.y += 1;
    }
    if (IsKeyDown(KEY_A)) {
      velocity.x -= 1;
    }
    if (IsKeyDown(KEY_D)) {
      velocity.x += 1;
    }

    velocity.x = std::max(-1.0f, std::min(1.0f, velocity.x));
    velocity.y = std::max(-1.0f, std::min(1.0f, velocity.y));

    // normalize force if it exceeds 1
    float mag = std::sqrt(pow(velocity.x, 2) + pow(velocity.y, 2));
    if (mag > 1) {
      velocity.x = velocity.x / mag;
      velocity.y = velocity.y / mag;
    }

    // std::cout << mag << " " << force.x << " " << force.y << " " << std::endl;
    velocity.x *= 0.9f;
    velocity.y *= 0.9f;

    if (std::abs(velocity.x) < epsilon) {
      velocity.x = 0;
    }
    if (std::abs(velocity.y) < epsilon) {
      velocity.y = 0;
    }

    playerCoords.x += velocity.x * forceMult;
    playerCoords.y += velocity.y * forceMult;

    playerCoords.x = std::max(0.0f, std::min(w - playerSize.x, playerCoords.x));
    playerCoords.y = std::max(0.0f, std::min(h - playerSize.y, playerCoords.y));

    // drawing
    BeginDrawing();

    // Setup the back buffer for drawing (clear color and depth buffers)
    ClearBackground(BLACK);
    h = GetScreenHeight();
    w = GetScreenWidth();
    {
      int size = 32;
      int i = size;
      while (i < h) {
        DrawLine(0, i, w, i, DGRAY);
        i += size;
      }
      i = size;
      while (i < w) {
        DrawLine(i, 0, i, h, DGRAY);
        i += size;
      }
    }

    for (int i = 0; i < terrainMap.positions.size(); i++) {
      for (int j = 0; j < terrainMap.positions[i].size(); j++) {
        switch (terrainMap.positions[i][j]) {
        case 1:
          spreadsheet.Draw("wall", {j * 32.0f, i * 32.0f});
          break;
        case 2:
          spreadsheet.Draw("grass", {j * 32.0f, i * 32.0f});
          break;
        case 3:
          spreadsheet.Draw("water", {j * 32.0f, i * 32.0f});
          break;
        default:
          break;
        }
      }
    }

    DrawRectangle(playerCoords.x, playerCoords.y, playerSize.x, playerSize.y,
                  GREEN);
    float mult = 100;
    DrawLine(playerCoords.x + playerSize.x / 2,
             playerCoords.y + playerSize.y / 2,
             playerCoords.x + playerSize.x / 2 + velocity.x * mult,
             playerCoords.y + playerSize.y / 2 + velocity.y * mult, RED);

    // end the frame and get ready for the next one  (display frame, poll input,
    // etc...)
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
