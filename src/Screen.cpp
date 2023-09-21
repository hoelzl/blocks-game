#include "Screen.h"
#include "App.h"
#include "raylib.h"
#include "raymath.h"
#include <cstdio>

void Screen::DrawFrame(const App& app) const {
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        switch (screenType) {

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

void Screen::UpdateGameState(App& app) {
    switch (screenType) {
    case LOGO: {
        ++app.framesCounter;

        if (app.framesCounter > LOGO_SCREEN_DURATION_IN_FRAMES) {
            screenType = TITLE;
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
            screenType = GAMEPLAY;
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
            screenType = ENDING;
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
            screenType = ENDING;
            break;
        }
        if (IsKeyPressed('L')) {
            app.player.lives = 0;
            app.gameResult = 0;
            screenType = ENDING;
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
                            screenType = ENDING;
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
                                        screenType = ENDING;
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
                    screenType = ENDING;
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
            screenType = TITLE;
        }
        break;
    }
    }
}
