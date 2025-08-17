#pragma once

#include "raylib.h"

#define GRID_SIZE 32

inline const Color DGRAY = {30, 30, 30, 255};

enum class GameState { PLAY, GAME_OVER, EDITOR };
