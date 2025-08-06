#include "raylib.h"
#include "resource_dir.h" // utility header for SearchAndSetResourceDir

#include <cmath>
#include <iostream>
#include <string>

using namespace std::string_literals;

Color DGRAY = {30, 30, 30, 255};

int main() {
  // Tell the window to use vsync and work on high DPI displays
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);

  // Create the window and OpenGL context
  InitWindow(1280, 800, "Hello Raylib");
  SetTargetFPS(60);

  // Utility function from resource_dir.h to find the resources folder and set
  // it as the current working directory so we can load from it
  SearchAndSetResourceDir("resources");

  // Load a texture from the resources directory
  Texture wabbit = LoadTexture("wabbit_alpha.png");

  int h = GetScreenHeight();
  int w = GetScreenWidth();
  Vector2 playerSize = {50.0f, 50.0f};
  Vector2 playerCoords = {w / 2.0f - playerSize.x / 2,
                          h / 2.0f - playerSize.y / 2};
  float velocity = 10.0f;
  Vector2 force = {0.0f, 0.0f};
  float epsilon = 0.01f;

  // game loop
  while (!WindowShouldClose()) // run the loop untill the user presses ESCAPE or
                               // presses the Close button on the window
  {
    if (IsKeyDown(KEY_W)) {
      force.y -= 1;
    }
    if (IsKeyDown(KEY_S)) {
      force.y += 1;
    }
    if (IsKeyDown(KEY_A)) {
      force.x -= 1;
    }
    if (IsKeyDown(KEY_D)) {
      force.x += 1;
    }

    force.x = std::max(-1.0f, std::min(1.0f, force.x));
    force.y = std::max(-1.0f, std::min(1.0f, force.y));

    // normalize force if it exceeds 1
    float mag = std::sqrt(pow(force.x, 2) + pow(force.y, 2));
    if (mag > 1) {
      force.x = force.x / mag;
      force.y = force.y / mag;
    }

    std::cout << mag << " " << force.x << " " << force.y << " " << std::endl;
    force.x *= 0.9f;
    force.y *= 0.9f;

    if (std::abs(force.x) < epsilon) {
      force.x = 0;
    }
    if (std::abs(force.y) < epsilon) {
      force.y = 0;
    }

    playerCoords.x += force.x * velocity;
    playerCoords.y += force.y * velocity;

    playerCoords.x = std::max(0.0f, std::min(w - playerSize.x, playerCoords.x));
    playerCoords.y = std::max(0.0f, std::min(h - playerSize.y, playerCoords.y));

    // drawing
    BeginDrawing();

    // Setup the back buffer for drawing (clear color and depth buffers)
    ClearBackground(BLACK);
    h = GetScreenHeight();
    w = GetScreenWidth();
    for (int i = 0; i < 10; i++) {
      DrawLine(0, i * h / 10, w, i * h / 10, DGRAY);
      DrawLine(i * w / 10, 0, i * w / 10, h, DGRAY);
    }

    // draw some text using the default font
    DrawText(("123"s + "456"s).c_str(), 200, 200, 20, DGRAY);

    // draw our texture to the screen
    DrawTexture(wabbit, 400, 200, WHITE);

    DrawRectangle(playerCoords.x, playerCoords.y, playerSize.x, playerSize.y,
                  GREEN);
    float mult = 100;
    DrawLine(playerCoords.x + playerSize.x / 2,
             playerCoords.y + playerSize.y / 2,
             playerCoords.x + playerSize.x / 2 + force.x * mult,
             playerCoords.y + playerSize.y / 2 + force.y * mult, RED);

    // end the frame and get ready for the next one  (display frame, poll input,
    // etc...)
    EndDrawing();
  }

  // cleanup
  // unload our texture so it can be cleaned up
  UnloadTexture(wabbit);

  // destroy the window and cleanup the OpenGL context
  CloseWindow();
  return 0;
}
