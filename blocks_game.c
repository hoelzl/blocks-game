#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
/***********************************************************************************
 * Example based on the raylib-intro-course, with support for emscripten.
 * CMake support is based on the CMakeLists.txt file from the raylib project.
 *
 * See <https://github.com/raysan5/raylib-intro-course> for the tutorial.
 */

#include "raylib.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define PLAYER_LIVES 5
#define BRICKS_LINES 5
#define BRICKS_PER_LINE 20
#define BRICKS_POSITION_Y 50

typedef enum GameScreen {
    LOGO, TITLE, GAMEPLAY, ENDING
} GameScreen;

typedef struct Player {
    Vector2 position;
    Vector2 speed;
    Vector2 size;
    Rectangle bounds;
    int lives;
} Player;

typedef struct Ball {
    Vector2 position;
    Vector2 speed;
    float radius;
    bool active;
} Ball;

typedef struct Brick {
    Vector2 position;
    Vector2 size;
    Rectangle bounds;
    int resistance;
    bool active;
} Brick;

GameScreen screen = LOGO;
Player player = {0};
Ball ball = {0};
Brick bricks[BRICKS_LINES][BRICKS_PER_LINE] = {0};

void UpdateDrawFrame();     // Update and Draw one frame

int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "PROJECT: BLOCKS GAME");

    // Initialize player
    player.position = (Vector2) {GetScreenWidth() / 2.0f, GetScreenHeight() - 50.0f};
    player.speed = (Vector2) {8.0f, 8.0f};
    player.size = (Vector2) {100, 24};
    player.lives = PLAYER_LIVES;

    // Initialize ball
    ball.radius = 10.0f;
    ball.active = false;
    ball.position = (Vector2) {player.position.x + player.size.x / 2, player.position.y - ball.radius * 2};
    ball.speed = (Vector2) {4.0f, 4.0f};

    // Initialize bricks
    for (int j = 0; j < BRICKS_LINES; ++j) {
        for (int i = 0; i < BRICKS_PER_LINE; ++i) {
            bricks[j][i].size = (Vector2) {(float) GetScreenWidth() / BRICKS_PER_LINE, 20};
            bricks[j][i].position = (Vector2) {bricks[j][i].size.x * i, BRICKS_POSITION_Y + bricks[j][i].size.y * j};
            bricks[j][i].bounds = (Rectangle) {bricks[j][i].position.x, bricks[j][i].position.y, bricks[j][i].size.x,
                                               bricks[j][i].size.y};
            bricks[j][i].active = true;
        }
    }

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
void UpdateDrawFrame() {

    static int framesCounter = 0;
    static int gameResult = -1;     // -1 = Game not finished, 1 = Win, 0 = Lose
    static bool gamePaused = false;

    switch (screen) {
        case LOGO: {
            ++framesCounter;

            if (framesCounter > 180) {
                screen = TITLE;
                framesCounter = 0;
            }
            break;
        }
        case TITLE: {
            // TODO: Update TITLE screen data here!
            ++framesCounter;
            if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
                screen = GAMEPLAY;
            }
            break;
        }
        case GAMEPLAY: {
            if (!gamePaused) {
                // TODO: Gameplay logic!
            }
            if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
                screen = ENDING;
            }
            break;
        }
        case ENDING: {
            // TODO: Update END screen data here!

            ++framesCounter;

            if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
                screen = TITLE;
            }
            break;
        }
    }

    BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        switch (screen) {

            case LOGO: {
                DrawText("LOGO SCREEN", 20, 20, 40, LIGHTGRAY);
                DrawText("WAIT for 3 SECONDS...", 290, 220, 20, GRAY);
                break;
            }
            case TITLE: {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), GREEN);
                DrawText("TITLE SCREEN", 20, 20, 40, DARKGREEN);

                if ((framesCounter / 30) % 2) {
                    DrawText("PRESS [ENTER] or TAP to START",
                             GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] or TAP to START", 20) / 2,
                             GetScreenHeight() / 2 + 60, 20,
                             DARKGRAY);
                }
                break;
            }
            case GAMEPLAY: {
                DrawRectangleV(player.position, player.size, BLACK);
                DrawCircleV(ball.position, ball.radius, MAROON);

                // Draw bricks
                for (int j = 0; j < BRICKS_LINES; ++j) {
                    for (int i = 0; i < BRICKS_PER_LINE; ++i) {
                        if (bricks[j][i].active) {
                            if ((i + j) % 2 == 0) {
                                DrawRectangleV(bricks[j][i].position, bricks[j][i].size, GRAY);
                            } else {
                                DrawRectangleV(bricks[j][i].position, bricks[j][i].size, DARKGRAY);
                            }
                        }
                    }
                }
                // Draw GUI: player lives
                for (int i = 0; i < player.lives; ++i) {
                    DrawRectangle(20 + 40 * i, GetScreenHeight() - 30, 35, 10, LIGHTGRAY);
                }
                // Draw pause message when required
                if (gamePaused) {
                    // DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(RAYWHITE, 0.8f));
                    DrawText("GAME PAUSED", GetScreenWidth() / 2 - MeasureText("GAME PAUSED", 40) / 2,
                             GetScreenHeight() / 2 - 40, 40, GRAY);
                }
                break;
            }
            case ENDING: {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLUE);
                DrawText("ENDING SCREEN", 20, 20, 40, DARKBLUE);
                if ((framesCounter / 30) % 2 == 0) {
                    DrawText("PRESS ENTER or TAP to RETURN to TITLE SCREEN",
                             GetScreenWidth() / 2 - MeasureText("Press ENTER or TAP to RETURN to TITLE SCREEN", 20) / 2,
                             GetScreenHeight() / 2 + 80, 20, DARKBLUE);
                }
                break;
            }
        }
    }
    EndDrawing();
    //----------------------------------------------------------------------------------
}
