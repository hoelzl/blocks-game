#pragma once
#ifndef BLOCKS_GAME_SCREEN_H
#define BLOCKS_GAME_SCREEN_H

#include <cstdio>
#include <memory>
#include "raylib.h"
#include "Common.h"

class App;

class Screen {
public:
    GameScreenType screenType{LOGO};

    static float GetScreenHeightFloat() {
        return static_cast<float>(GetScreenHeight());
    }
    static float GetScreenWidthFloat() {
        return static_cast<float>(GetScreenWidth());
    }
    void DrawFrame(const App& app) const;
    void UpdateGameState(App& app);
};

#endif //BLOCKS_GAME_SCREEN_H
