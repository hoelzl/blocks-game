#pragma once
#ifndef BLOCKS_GAME_BLOCKS_GAME_H
#define BLOCKS_GAME_BLOCKS_GAME_H

#include "raymath.h"
#include "raylib.h"
#include <cstdio>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define DEBUG_GAME_FLOW

#ifdef DEBUG_GAME_FLOW
#define LOGO_SCREEN_DURATION_IN_FRAMES 60
#define NUM_PLAYER_LIVES 2
#define BRICKS_LINES 2
#define BRICKS_PER_LINE 8
#define MASTER_VOLUME 0.0f
#else
#define LOGO_SCREEN_DURATION_IN_FRAMES 180
#define NUM_PLAYER_LIVES 5
#define BRICKS_LINES 5
#define BRICKS_PER_LINE 20
#define MASTER_VOLUME 0.2f
#endif

#define BRICKS_POSITION_Y 50
#define BRICK_RESISTANCE 1

#define SOUND_FX_VOLUME 0.2f
#define MUSIC_STREAM_VOLUME 0.8f

enum GameScreenType {
    LOGO, TITLE, GAMEPLAY, ENDING
};

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

struct App {
    GameScreenType screen = LOGO;
    Player player = {0};
    Ball ball = {0};
    Brick bricks[BRICKS_LINES][BRICKS_PER_LINE] = {0};
    float brickScale = 1.0f;
    int numActiveBricks = 0;

    Texture2D texLogo{};
    Texture2D texBall{};
    Texture2D texPaddle{};
    Texture2D texBrick{};

    Font font{};

    Sound fxStart{};
    Sound fxBounce{};
    Sound fxExplode{};

    Music music{};

    int framesCounter = 0;
    int gameResult = -1;     // -1 = Game not finished, 1 = Win, 0 = Lose
    bool gamePaused = false;

    static float GetScreenHeightFloat() {
        return (float) GetScreenHeight();
    }

    static float GetScreenWidthFloat() {
        return (float) GetScreenWidth();
    }

    void InitAppAndRunGameLoop() {
        const int screenWidth = 800;
        const int screenHeight = 450;

        InitWindow(screenWidth, screenHeight, "PROJECT: BLOCKS GAME");
        LoadGraphicsResources();

        InitAudioDevice();
        SetMasterVolume(MASTER_VOLUME);
        LoadSoundResources();
        PlayMusicStream(music);

        InitializePlayer();
        InitializeBall();
        InitializeBricks();

        RunGameLoop();

        UnloadGraphicsResources();
        UnloadSoundResources();
        CloseAudioDevice();
        CloseWindow();
    }

    void UnloadSoundResources() const {
        UnloadSound(fxStart);
        UnloadSound(fxBounce);
        UnloadSound(fxExplode);

        UnloadMusicStream(music);
    }

    void UnloadGraphicsResources() const {// Unload textures
        UnloadTexture(texBall);
        UnloadTexture(texPaddle);
        UnloadTexture(texBrick);

        UnloadFont(font);
    }

    void RunGameLoop() {
#if defined(PLATFORM_WEB)
        emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
        SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
//--------------------------------------------------------------------------------------

        // Main game loop
        while (!WindowShouldClose())    // Detect window close button or ESC key
        {
            UpdateGameStateAndDrawFrame();
        }
#endif
    }

    void InitializeBricks() {// Initialize bricks
        brickScale = GetScreenWidthFloat() / ((float) BRICKS_PER_LINE * (float) texBrick.width);
        for (int j = 0; j < BRICKS_LINES; ++j) {
            for (int i = 0; i < BRICKS_PER_LINE; ++i) {
                bricks[j][i].size = Vector2{(float) texBrick.width * brickScale, (float) texBrick.height * brickScale};
                bricks[j][i].position = Vector2{bricks[j][i].size.x * (float) i,
                                                BRICKS_POSITION_Y + bricks[j][i].size.y * (float) j};
                bricks[j][i].bounds = Rectangle{bricks[j][i].position.x, bricks[j][i].position.y, bricks[j][i].size.x,
                                                bricks[j][i].size.y};
                bricks[j][i].resistance = BRICK_RESISTANCE;
                bricks[j][i].active = true;
                ++numActiveBricks;
            }
        }
    }

    void InitializeBall() {// Initialize ball
        ball.radius = 10.0f;
        ball.active = false;
        ball.position = Vector2{player.position.x + player.size.x / 2 - ball.radius,
                                player.position.y - ball.radius * 2};
        ball.speed = Vector2{0.0f, 0.0f};
    }

    void InitializePlayer() {// Initialize player
        player.position = Vector2{GetScreenWidthFloat() / 2.0f, GetScreenHeightFloat() * 7.0f / 8.0f};
        player.speed = Vector2{8.0f, 8.0f};
        player.size = Vector2{100, 24};
        player.lives = NUM_PLAYER_LIVES;
    }

    void LoadSoundResources() {
        fxStart = LoadSound("resources/start.wav");
        fxBounce = LoadSound("resources/bounce.wav");
        fxExplode = LoadSound("resources/explosion.wav");

        SetSoundVolume(fxStart, SOUND_FX_VOLUME);
        SetSoundVolume(fxBounce, SOUND_FX_VOLUME);
        SetSoundVolume(fxExplode, SOUND_FX_VOLUME);

        music = LoadMusicStream("resources/go-wild.mp3");

        SetMusicVolume(music, MUSIC_STREAM_VOLUME);
    }

    void LoadGraphicsResources() {// Note: Load resources after window initialization (OpenGL context is required)
        texLogo = LoadTexture("resources/raylib_logo.png");
        texBall = LoadTexture("resources/ball.png");
        texPaddle = LoadTexture("resources/paddle.png");
        texBrick = LoadTexture("resources/brick.png");

        font = LoadFont("resources/setback.png");
    }

    void UpdateGameStateAndDrawFrame() {
        UpdateGameState(*this);
        UpdateMusicStream(music);
        DrawFrame(*this);
    }

    static void DrawFrame(const App& app) {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            switch (app.screen) {

            case LOGO: {
                DrawTexture(app.texLogo, GetScreenWidth() / 2 - app.texLogo.width / 2,
                            GetScreenHeight() / 2 - app.texLogo.height / 2,
                            WHITE);
                char text[80] = {0};
                sprintf_s(text, 80, "WAIT for %.1f SECONDS...", 3.0f - (float) app.framesCounter / 60.0f);
                DrawText(text, GetScreenWidth() / 2 - MeasureText(text, 2), GetScreenHeight() * 7 / 8,
                         20,
                         GRAY);
                break;
            }
            case TITLE: {
                DrawTextEx(app.font, "BLOCKS", Vector2{100, 80}, 160, 10, MAROON);

                if ((app.framesCounter / 30) % 2) {
                    DrawText("PRESS [ENTER] OR TAP TO START",
                             GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] OR TAP TO START", 20) / 2,
                             GetScreenHeight() / 2 + 60, 20,
                             DARKGRAY);
                }
                break;
            }
            case GAMEPLAY: {
                // Draw bricks
                for (int j = 0; j < BRICKS_LINES; ++j) {
                    for (int i = 0; i < BRICKS_PER_LINE; ++i) {
                        if (app.bricks[j][i].active) {
                            if ((i + j) % 2 == 0) {
                                DrawTextureEx(app.texBrick, app.bricks[j][i].position, 0.0f, app.brickScale, GRAY);
                            } else {
                                DrawTextureEx(app.texBrick, app.bricks[j][i].position, 0.0f, app.brickScale, DARKGRAY);
                            }
                        }
                    }
                }

                // Draw player
                DrawTextureEx(app.texPaddle, app.player.position, 0.0f, 1.0f, WHITE);
                // Draw ball
                DrawTextureEx(app.texBall, app.ball.position, 0.0f, 1.0f, MAROON);

                // Draw GUI: player lives
                for (int i = 0; i < app.player.lives; ++i) {
                    DrawRectangle(20 + 40 * i, GetScreenHeight() - 30, 35, 10, LIGHTGRAY);
                }

                // Draw pause message when required
                if (app.gamePaused) {
                    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(RAYWHITE, 0.8f));
                    DrawText("GAME PAUSED", GetScreenWidth() / 2 - MeasureText("GAME PAUSED", 40) / 2,
                             GetScreenHeight() / 2 - 40, 40, GRAY);
                }
                break;
            }
            case ENDING: {
                if (app.gameResult == 0) {
                    if (app.numActiveBricks > BRICKS_PER_LINE * BRICKS_LINES / 2) {
                        DrawTextEx(app.font,
                                   "YOU LOSE!",
                                   Vector2{
                                       GetScreenWidthFloat() / 2.0f -
                                           MeasureTextEx(app.font, "YOU_LOSE!", 80, 5).x / 2.0f,
                                       100},
                                   80,
                                   5, MAROON);

                    } else {
                        DrawTextEx(app.font,
                                   "GAME OVER",
                                   Vector2{
                                       GetScreenWidthFloat() / 2.0f -
                                           MeasureTextEx(app.font, "GAME OVER", 80, 5).x / 2.0f,
                                       100},
                                   80,
                                   5, MAROON);
                    }
                } else if (app.gameResult == 1) {
                    DrawTextEx(app.font,
                               "YOU WIN!",
                               Vector2{
                                   GetScreenWidthFloat() / 2.0f -
                                       MeasureTextEx(app.font, "YOU WIN!", 80, 5).x / 2.0f,
                                   100},
                               80,
                               5, MAROON);
                } else {
                    DrawTextEx(app.font,
                               "WHAT HAPPENED?",
                               Vector2{
                                   GetScreenWidthFloat() / 2.0f -
                                       MeasureTextEx(app.font, "WHAT HAPPENED?", 80, 5).x / 2.0f,
                                   100},
                               80,
                               5, MAROON);
                }

                if ((app.framesCounter / 30) % 2 == 0) {
                    DrawText("PRESS [ENTER] TO PLAY AGAIN",
                             GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
                             GetScreenHeight() / 2 + 80, 20, GRAY);
                }
                break;
            }
            }
        }
        EndDrawing();
    }

    static void UpdateGameState(App& app) {
        switch (app.screen) {
        case LOGO: {
            ++app.framesCounter;

            if (app.framesCounter > LOGO_SCREEN_DURATION_IN_FRAMES) {
                app.screen = TITLE;
                app.framesCounter = 0;
            }
            break;
        }
        case TITLE: {
            // Reset everything if we come back here after a game over
            if (app.gameResult != -1) {
                app.gameResult = -1;
                app.numActiveBricks = 0;
                for (int j = 0; j < BRICKS_LINES; ++j) { // NOLINT(*-loop-convert)
                    for (int i = 0; i < BRICKS_PER_LINE; ++i) {
                        app.bricks[j][i].resistance = BRICK_RESISTANCE;
                        app.bricks[j][i].active = true;
                        ++app.numActiveBricks;
                    }
                }
                app.player.lives = NUM_PLAYER_LIVES;
                app.player.position = Vector2{GetScreenWidthFloat() / 2.0f, GetScreenHeightFloat() * 7.0f / 8.0f};
                app.ball.position.x = app.player.position.x + app.player.size.x / 2 - app.ball.radius;
                app.ball.position.y = app.player.position.y - app.ball.radius * 2;
                app.ball.active = false;
            }

            ++app.framesCounter;
            if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
                app.screen = GAMEPLAY;
                PlaySound(app.fxStart);
            }
            break;
        }
        case GAMEPLAY: {
            if (IsKeyPressed('P')) {
                app.gamePaused = !app.gamePaused;
            }
            if (IsKeyPressed('Q')) {
                app.player.lives = 0;
                app.gameResult = app.numActiveBricks == 0 ? 1 : 0;
                app.screen = ENDING;
                break;
            }
            if (IsKeyPressed('W')) {
                for (int j = 0; j < BRICKS_LINES; ++j) { // NOLINT(*-loop-convert)
                    for (int i = 0; i < BRICKS_PER_LINE; ++i) {
                        app.bricks[j][i].active = false;
                        --app.numActiveBricks;
                    }
                }
                app.player.lives = 0;
                app.gameResult = 1;
                app.screen = ENDING;
                break;
            }
            if (IsKeyPressed('L')) {
                app.player.lives = 0;
                app.gameResult = 0;
                app.screen = ENDING;
                break;
            }
            if (IsKeyPressed('K')) {
                for (int j = 0; j < BRICKS_LINES; ++j) { // NOLINT(*-loop-convert)
                    for (int i = 0; i < BRICKS_PER_LINE; ++i) {
                        int rand = GetRandomValue(0, 10);
                        if (rand < 5 && app.bricks[j][i].active) {
                            app.bricks[j][i].resistance = 0;
                            app.bricks[j][i].active = false;
                            --app.numActiveBricks;
                            if (app.numActiveBricks == 0) {
                                app.screen = ENDING;
                                app.gameResult = 1;
                                break;
                            }
                        }
                    }
                }
            }

            if (!app.gamePaused) {
                if (IsKeyDown(KEY_LEFT))
                    app.player.position.x -= app.player.speed.x;
                if (IsKeyDown(KEY_RIGHT))
                    app.player.position.x += app.player.speed.x;

                if ((app.player.position.x) <= 0) {
                    app.player.position.x = 0;
                }
                if ((app.player.position.x + app.player.size.x) >= GetScreenWidthFloat()) {
                    app.player.position.x = GetScreenWidthFloat() - app.player.size.x;
                }

                app.player.bounds =
                    Rectangle{app.player.position.x, app.player.position.y, app.player.size.x, app.player.size.y};

                if (app.ball.active) {
                    // Ball movement logic
                    app.ball.position.x += app.ball.speed.x;
                    app.ball.position.y += app.ball.speed.y;

                    // Collision logic: ball vs. app.screen limits
                    if ((app.ball.position.x + 2 * app.ball.radius) >= GetScreenWidthFloat()) {
                        app.ball.position.x = GetScreenWidthFloat() - 2 * app.ball.radius;
                        app.ball.speed.x *= -1;
                    } else if (app.ball.position.x <= 0) {
                        app.ball.speed.x *= -1;
                        app.ball.position.x = 0;
                    }

                    if (app.ball.position.y <= 0) {
                        app.ball.speed.y *= -1;
                    }

                    // Collision logic: ball vs. app.player
                    Vector2 ballCenter =
                        Vector2{app.ball.position.x + app.ball.radius, app.ball.position.y + app.ball.radius};
                    if (CheckCollisionCircleRec(ballCenter, app.ball.radius, app.player.bounds)) {
                        if (app.ball.speed.y > 0) {
                            app.ball.speed.y *= -1;
                        }
                        app.ball.speed.x =
                            (ballCenter.x - app.player.position.x - app.player.size.x / 2) / (app.player.size.x / 2)
                                * 5.0f;
                        PlaySound(app.fxBounce);
                    }

                    // Collision logic: ball vs. app.bricks
                    for (int j = 0; j < BRICKS_LINES; ++j) { // NOLINT(*-loop-convert)
                        for (int i = 0; i < BRICKS_PER_LINE; ++i) {
                            if (app.bricks[j][i].active) {
                                if (CheckCollisionCircleRec(ballCenter, app.ball.radius, app.bricks[j][i].bounds)) {
                                    --app.bricks[j][i].resistance;
                                    if (app.bricks[j][i].resistance <= 0) {
                                        app.bricks[j][i].active = false;
                                        --app.numActiveBricks;
                                        PlaySound(app.fxExplode);
                                        if (app.numActiveBricks == 0) {
                                            app.screen = ENDING;
                                            app.gameResult = 1;
                                        }
                                    } else {
                                        PlaySound(app.fxBounce);
                                    }

                                    app.ball.speed.y *= -1;
                                    break;
                                }
                            }
                        }
                    }

                    // Game ending logic
                    if ((ballCenter.y + app.ball.radius) >= GetScreenHeightFloat()) {
                        app.ball.position.x = app.player.position.x + app.player.size.x / 2 - app.ball.radius;
                        app.ball.position.y = app.player.position.y - app.ball.radius * 2;
                        app.ball.active = false;
                        app.player.lives--;
                    }

                    if (app.player.lives <= 0) {
                        app.screen = ENDING;
                        app.player.lives = NUM_PLAYER_LIVES;
                        app.gameResult = app.numActiveBricks == 0 ? 1 : 0;
                        app.framesCounter = 0;
                    }
                } else {
                    // Reset ball position
                    app.ball.position.x = app.player.position.x + app.player.size.x / 2 - app.ball.radius;
                    if (IsKeyPressed(KEY_SPACE)) {
                        const float angle = (float) GetRandomValue(-300, 300) / 10.0f;
                        app.ball.speed = Vector2Rotate(Vector2{0.0f, -5.0f}, angle * DEG2RAD);
                        app.ball.active = true;
                    }
                }
            }
            break;
        }
        case ENDING: {
            ++app.framesCounter;

            if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
                app.screen = TITLE;
            }
            break;
        }
        }
    }
};
#endif //BLOCKS_GAME_BLOCKS_GAME_H
