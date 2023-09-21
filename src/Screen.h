#pragma once
#ifndef BLOCKS_GAME_SCREEN_H
#define BLOCKS_GAME_SCREEN_H

#include <cstdio>
#include "raylib.h"
#include "App.h"

class Screen {

public:
    static float GetScreenHeightFloat() {
        return static_cast<float>(GetScreenHeight());
    }
    static float GetScreenWidthFloat() {
        return static_cast<float>(GetScreenWidth());
    }
    static void DrawFrame(const App& app);
    static void UpdateGameState(App& app);
};

#endif //BLOCKS_GAME_SCREEN_H
