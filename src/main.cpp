#include "config.h"
#include "gizmos.h"
#include "player.h"
#include "raylib.h"
#include "resource_dir.h"
#include "target.h"
#include "terrain.h"
#include "texture_drawer.h"
#include "utils.h"
#include <cstdlib>
#include <vector>

int main() {
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);
  InitWindow(1280, 800, "Hello superpumped");
  SearchAndSetResourceDir("resources");
  InitAudioDevice();

  TextureDrawer spritesheet("spritesheet.png",
                            {
                                {"wall", {0, 1}},
                                {"grass", {0, 2}},
                                {"water", {0, 3}},
                            },
                            32);
  Config::Get().terrain = Terrain();
  Config::Get().player = Player();
  Player &player = Config::Get().player;
  Gizmos gizmos;

  Config::Get().terrain.load("terrain.csv");
  // Config::Get().terrain.generate_tiles();
  Sound track = LoadSound("Sub Urban - Cradles [NCS Release].mp3");
  float h = GetScreenHeight();
  float w = GetScreenWidth();
  Camera2D camera = {0};
  camera.target =
      (Vector2){player.translation.x + 20.0f, player.translation.y + 20.0f};
  camera.offset = (Vector2){w / 2, h / 2};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;
  std::vector<std::vector<double>> times = {
      {3.037974684},
      // {4.556962025},
      // {6.075949367},
      // {7.594936709, 8.35443038, 9.113924051},
      // {10.63291139},
      // {12.15189873, 12.91139241, 13.67088608},
      // {15.18987342, 15.56962025, 15.94936709, 16.32911392},
      // {18.2278481, 18.60759494, 18.98734177, 19.36708861},
      // {21.26582278, 21.64556962, 22.02531646, 22.40506329},
      // {24.30379747, 24.6835443, 25.06329114, 25.44303797},
      // {27.34177215, 27.72151899, 28.10126582, 28.48101266},
      // {30.37974684, 30.75949367, 31.13924051, 31.51898734},
      // {33.41772152, 33.79746835, 34.17721519, 34.55696203},
      // {36.4556962, 36.83544304, 37.21518987, 37.59493671}
  };

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_R)) {
      Config::Get().targets.clear();
      Config::Get().enemies.clear();
      Config::Get().score = 0;
      Config::Get().scoreMult = 1;
      player.Reset();
      player.translation = {h / 2, w / 2};
      StopSound(track);

      PlaySound(track);

      for (size_t i = 0; i < std::size(times); i++) {
        float el_h = GetRandomValue(200, 300);
        float el_w = GetRandomValue(200, 300);

        float phi = Utils::Deg2Rad(static_cast<float>(GetRandomValue(0, 360)));
        float thetaStart =
            Utils::Deg2Rad(static_cast<float>(GetRandomValue(0, 360)));
        float thetaWidth =
            Utils::Deg2Rad(static_cast<float>(GetRandomValue(30, 160)));
        float thetaEnd = thetaStart + thetaWidth;

        int steps = times[i].size();

        Vector2 center{static_cast<float>(i * 200),
                       static_cast<float>(h) * 0.5f};
        if (i % 4 == 0) {
          Config::Get().enemies.push_back(Enemy(center, 300));
        }
        std::vector<Vector2> points = Utils::GenerateRotatedEllipseSector(
            el_w, el_h, phi, thetaStart, thetaEnd, steps);
        for (int j = 0; j < size(times[i]); j++) {
          Config::Get().targets.push_back(
              Target(Utils::add(points[j], center), times[i][j], 4, 1.2f));
        }
      }
    }
    for (Enemy &enemy : Config::Get().enemies) {
      enemy.Update();
    };
    player.Update();
    gizmos.Update();

    camera.target = (Vector2){player.translation.x, player.translation.y};
    BeginDrawing();
    BeginMode2D(camera);
    ClearBackground(BLACK);
    gizmos.Draw();
    for (Target &target : Config::Get().targets) {
      target.Update();
    };
    for (Tile &tile : Config::Get().terrain.tiles) {
      tile.Draw(spritesheet);
    }
    for (Target &target : Config::Get().targets) {
      target.Draw();
    };
    for (Enemy &enemy : Config::Get().enemies) {
      enemy.Draw();
    };
    player.Draw();
    EndMode2D();

    DrawText(TextFormat("Score %.2f x %.2f", Config::Get().score,
                        Config::Get().scoreMult),
             w / 2 - 100, 20, 20, YELLOW);
    gizmos.DrawUI();
    player.DrawUI();
    EndDrawing();
  }
  UnloadSound(track);
  CloseAudioDevice();
  CloseWindow();

  return 0;
}
