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

typedef enum GameScreen {
    LOGO, TITLE, GAMEPLAY, ENDING
} GameScreen;

void UpdateDrawFrame(GameScreen *screen);     // Update and Draw one frame

int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "PROJECT: BLOCKS GAME");

    GameScreen screen = LOGO;

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame(&screen);
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
void UpdateDrawFrame(GameScreen *screen) {

    static int framesCounter = 0;
    static int gameResult = -1;     // -1 = Game not finished, 1 = Win, 0 = Lose
    static bool gamePaused = false;

    switch (*screen) {
        case LOGO: {
            ++framesCounter;

            if (framesCounter > 180) {
                *screen = TITLE;
                framesCounter = 0;
            }
            break;
        }
        case TITLE: {
            // TODO: Update TITLE screen data here!
            ++framesCounter;
            if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
                *screen = GAMEPLAY;
            }
            break;
        }
        case GAMEPLAY: {
            if (!gamePaused) {
                // TODO: Gameplay logic!
            }
            if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
                *screen = ENDING;
            }
            break;
        }
        case ENDING: {
            // TODO: Update END screen data here!

            ++framesCounter;

            if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
                *screen = TITLE;
            }
            break;
        }
    }

    BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        switch (*screen) {

            case LOGO: {
                DrawText("LOGO SCREEN", 20, 20, 40, LIGHTGRAY);
                DrawText("WAIT for 3 SECONDS...", 290, 220, 20, GRAY);
                break;
            }
            case TITLE: {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), GREEN);
                DrawText("TITLE SCREEN", 20, 20, 40, DARKGREEN);
                DrawText("PRESS ENTER or TAP to JUMP to GAMEPLAY SCREEN", 120, 220, 20, DARKGREEN);
                break;
            }
            case GAMEPLAY: {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), PURPLE);
                DrawText("GAMEPLAY SCREEN", 20, 20, 40, MAROON);
                DrawText("PRESS ENTER or TAP to JUMP to ENDING SCREEN", 130, 220, 20, MAROON);
                break;
            }
            case ENDING: {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLUE);
                DrawText("ENDING SCREEN", 20, 20, 40, DARKBLUE);
                DrawText("PRESS ENTER or TAP to RETURN to TITLE SCREEN", 120, 220, 20, DARKBLUE);
                break;
            }
        }
    }
    EndDrawing();
    //----------------------------------------------------------------------------------
}
