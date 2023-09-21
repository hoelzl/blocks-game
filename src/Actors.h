#pragma once
#ifndef BLOCKS_GAME_ACTORS_H
#define BLOCKS_GAME_ACTORS_H

#include "raylib.h"

struct Player {
    Vector2 position;
    Vector2 speed;
    Vector2 size;
    Rectangle bounds;
    int lives;
};

struct Ball {
    Vector2 position;
    Vector2 speed;
    float radius;
    bool active;
};

struct Brick {
    Vector2 position;
    Vector2 size;
    Rectangle bounds;
    int resistance;
    bool active;
};
#endif //BLOCKS_GAME_ACTORS_H
