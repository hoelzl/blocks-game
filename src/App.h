#pragma once
#ifndef BLOCKS_GAME_APP_H
#define BLOCKS_GAME_APP_H

#include "Common.h"
#include "Actors.h"
#include "Screen.h"
#include <memory>

class App {
public:
    explicit App(const char* title = "RAYLIB APP", int screenWidth = 800, int screenHeight = 450);
    virtual ~App();
    void InitAppAndRunGameLoop();

    Screen screen{};
    Player player{0};
    Ball ball{0};
    Brick bricks[BRICKS_LINES][BRICKS_PER_LINE]{0};
    float brickScale{1.0f};
    int numActiveBricks{0};

    Texture2D texLogo{};
    Texture2D texBall{};
    Texture2D texPaddle{};
    Texture2D texBrick{};

    Font font{};

    Sound fxStart{};
    Sound fxBounce{};
    Sound fxExplode{};

    Music music{};

    int framesCounter{0};
    int gameResult{-1};     // -1 = Game not finished, 1 = Win, 0 = Lose
    bool gamePaused{false};

    void UnloadSoundResources() const;
    void UnloadGraphicsResources() const;
    void RunGameLoop();
    void InitializeBricks();
    void InitializeBall();
    void InitializePlayer();
    void LoadSoundResources();
    void LoadGraphicsResources();
    void UpdateGameStateAndDrawFrame();
};

#endif //BLOCKS_GAME_APP_H
