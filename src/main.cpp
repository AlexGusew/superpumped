#include "raylib.h"
#include "resource_dir.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

using namespace std;
using namespace std::string_literals;

Color DGRAY = {30, 30, 30, 255};

#define GRID_SIZE 32

class Config {
public:
  bool gizmosEnabled = true;
  bool gizmosUIEnabled = true;

  static Config &Get() {
    static Config instance;
    return instance;
  }

  Config(const Config &) = delete;
  void operator=(const Config &) = delete;

private:
  Config() = default;
};

class GameObject {
public:
  Vector2 translation;
  Vector2 size;
  bool isCollider;

  virtual void Draw() {};
  virtual void DrawGizmos() {};
  virtual void Update() {};

  virtual ~GameObject() = default;
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
    texture = LoadTexture(path.c_str());
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

  void Unload() { UnloadTexture(texture); }
};

class Tile : public GameObject {
public:
  string id;

  Tile(string _id, Vector2 _translation) : id(_id) {
    translation = _translation;
  }

  void Draw(TextureDrawer &drawer) { drawer.Draw(id, translation); }
};

class Player : public GameObject {
public:
  float forceMult;
  float epsilon;
  Vector2 velocity;

  Player() {
    int h = GetScreenHeight();
    int w = GetScreenWidth();

    size = {50.0f, 50.0f};
    translation = {w / 2.0f - size.x / 2, h / 2.0f - size.y / 2};
    forceMult = 10.0f;
    velocity = {0.0f, 0.0f};
    epsilon = 0.01f;
  }

  void Update() override {
    int h = GetScreenHeight();
    int w = GetScreenWidth();

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

    velocity.x = std::clamp(velocity.x, -1.0f, 1.0f);
    velocity.y = std::clamp(velocity.y, -1.0f, 1.0f);

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

    translation.x += velocity.x * forceMult;
    translation.y += velocity.y * forceMult;

    translation.x = clamp(translation.x, 0.0f, w - size.x);
    translation.y = clamp(translation.y, 0.0f, h - size.y);
  }

  void Draw() override {
    DrawRectangle(translation.x, translation.y, size.x, size.y, GREEN);
    if (Config::Get().gizmosEnabled) {
      DrawGizmos();
    }
  }

  void DrawGizmos() override {
    float playerVelocityLen = 100;
    DrawLine(translation.x + size.x / 2, translation.y + size.y / 2,
             translation.x + size.x / 2 + velocity.x * playerVelocityLen,
             translation.y + size.y / 2 + velocity.y * playerVelocityLen, RED);
  }
};

class Gizmos {
private:
  bool panelOpen = false;

public:
  void Update() {
    if (IsKeyPressed(KEY_F1)) {
      Config::Get().gizmosUIEnabled = !Config::Get().gizmosUIEnabled;
    }
  }

  void Draw() {
    if (!Config::Get().gizmosUIEnabled) {
      return;
    }

    if (GuiButton({10, 10, 100, 20},
                  panelOpen ? "Hide Gizmos" : "Show Gizmos")) {
      panelOpen = !panelOpen;
    }

    if (panelOpen) {
      bool gizmosEnabled = Config::Get().gizmosEnabled;
      if (GuiCheckBox({20, 40, 20, 20}, "Draw gizmos", &gizmosEnabled)) {
        Config::Get().gizmosEnabled = gizmosEnabled;
      }
    }

    // Draw grid only if enabled
    if (Config::Get().gizmosEnabled) {
      int h = GetScreenHeight();
      int w = GetScreenWidth();
      int i = GRID_SIZE;
      while (i < h) {
        DrawLine(0, i, w, i, DGRAY);
        i += GRID_SIZE;
      }
      i = GRID_SIZE;
      while (i < w) {
        DrawLine(i, 0, i, h, DGRAY);
        i += GRID_SIZE;
      }
    }
  }
};

class Terrain {
private:
  string path;

public:
  vector<vector<int>> positions;
  vector<Tile> tiles;

  Terrain(const string &_path) : path(_path), positions({}), tiles({}) {}

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

  void generateTiles() {
    for (int i = 0; i < positions.size(); i++) {
      for (int j = 0; j < positions[i].size(); j++) {
        Vector2 translation = {static_cast<float>(j * GRID_SIZE),
                               static_cast<float>(i * GRID_SIZE)};
        switch (positions[i][j]) {
        case 1:
          tiles.push_back(Tile("wall", translation));
          break;
        case 2:
          tiles.push_back(Tile("grass", translation));
          break;
        case 3:
          tiles.push_back(Tile("water", translation));
          break;
        default:
          break;
        }
      }
    }
  }

  void clear() { positions.clear(); }
};

int main() {
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);

  InitWindow(1280, 800, "Hello superpumped");
  SetTargetFPS(60);

  SearchAndSetResourceDir("resources");

  TextureDrawer spritesheet("spritesheet.png",
                            {
                                {"wall", {0, 1}},
                                {"grass", {0, 2}},
                                {"water", {0, 3}},
                            },
                            32);
  Terrain terrain("terrain.csv");
  terrain.Load();
  terrain.generateTiles();

  Player player;
  Gizmos gizmos;

  while (!WindowShouldClose()) {
    player.Update();
    gizmos.Update();

    BeginDrawing();
    ClearBackground(BLACK);
    gizmos.Draw();
    for (Tile tile : terrain.tiles) {
      tile.Draw(spritesheet);
    }
    player.Draw();
    EndDrawing();
  }

  spritesheet.Unload();

  CloseWindow();
  return 0;
}
