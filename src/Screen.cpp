#include "Screen.h"
#include "App.h"
#include "raylib.h"
#include "raymath.h"
#include <cstdio>

void Screen::DrawCenteredText(const char* text, int posY, int fontSize, Color color) {
    DrawText(text, GetScreenWidth() / 2 - MeasureText(text, fontSize) / 2, posY, fontSize, color);
}

void Screen::DrawCenteredText(const App& app,
                              const std::string& fontName,
                              const char* text,
                              int posY,
                              int fontSize,
                              int spacing,
                              Color color) {
    const Font font = app.GetFont(fontName);
    DrawTextEx(font,
               text,
               Vector2{
                   Screen::GetWidth<float>() / 2.0f
                       - MeasureTextEx(font, text, static_cast<float>( fontSize), static_cast<float>( spacing)).x
                           / 2.0f,
                   static_cast<float>(posY)}, static_cast<float>(fontSize), static_cast<float>(spacing), color);

}

void Screen::DrawFlashingText(const App& app, const char* text, int posY, int fontSize, Color color) {
    if ((app.framesCounter / 30) % 2) {
        Screen::DrawCenteredText(text, posY, fontSize, color);
    }
}

void Screen::DrawFrame(const App& app) const {

    BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        screenImpl->DrawFrame(app);
    }
    EndDrawing();
}

void Screen::UpdateGameState(App& app) {

    if (screenImpl == nullptr) {
        screenImpl = std::make_unique<LogoScreen>();
    }
    auto nextScreen = screenImpl->UpdateGameState(app);
    if (nextScreen != nullptr) {
        screenImpl = std::move(nextScreen);
    }
}

std::unique_ptr<ScreenImpl> LogoScreen::UpdateGameState(App& app) {

    ++app.framesCounter;

    if (app.framesCounter > LOGO_SCREEN_DURATION_IN_FRAMES) {
        app.framesCounter = 0;
        return std::make_unique<TitleScreen>();
    }
    return {};
}

void LogoScreen::DrawFrame(const App& app) const {

    DrawTexture(app.GetTexture("logo"), GetScreenWidth() / 2 - app.GetTexture("logo").width / 2,
                GetScreenHeight() / 2 - app.GetTexture("logo").height / 2,
                WHITE);
    char text[80] = {0};
    sprintf_s(text, 80, "WAIT for %.1f SECONDS...", 3.0f - static_cast<float>(app.framesCounter) / 60.0f);
    Screen::DrawCenteredText(text, GetScreenHeight() * 7 / 8, 20, GRAY);
}

std::unique_ptr<ScreenImpl> TitleScreen::UpdateGameState(App& app) {
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
        app.player.position =
            Vector2{Screen::GetWidth<float>() / 2.0f, Screen::GetHeight<float>() * 7.0f / 8.0f};
        app.ball.position.x = app.player.position.x + app.player.size.x / 2 - app.ball.radius;
        app.ball.position.y = app.player.position.y - app.ball.radius * 2;
        app.ball.active = false;
    }

    ++app.framesCounter;
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
        PlaySound(app.GetSound("start"));
        return std::make_unique<GameplayScreen>();
    }
    return {};
}

void TitleScreen::DrawFrame(const App& app) const {

    Screen::DrawCenteredText(app, "setback", "BLOCKS", 80, 160, 10, MAROON);
    Screen::DrawFlashingText(app, "PRESS [ENTER] OR TAP TO START", GetScreenHeight() / 2 + 60, 20, DARKGRAY);
}

std::unique_ptr<ScreenImpl> GameplayScreen::UpdateGameState(App& app) {
    if (IsKeyPressed('P')) {
        app.gamePaused = !app.gamePaused;
    }
    auto forcedNewScreen{HandleDebugKeys(app)};
    if (forcedNewScreen != nullptr) {
        return forcedNewScreen;
    }

    if (!app.gamePaused) {
        if (IsKeyDown(KEY_LEFT))
            app.player.position.x -= app.player.speed.x;
        if (IsKeyDown(KEY_RIGHT))
            app.player.position.x += app.player.speed.x;

        if ((app.player.position.x) <= 0) {
            app.player.position.x = 0;
        }
        if ((app.player.position.x + app.player.size.x) >= Screen::GetWidth<float>()) {
            app.player.position.x = Screen::GetWidth<float>() - app.player.size.x;
        }

        app.player.bounds =
            Rectangle{app.player.position.x, app.player.position.y, app.player.size.x, app.player.size.y};

        if (app.ball.active) {
            // Ball movement logic
            app.ball.position.x += app.ball.speed.x;
            app.ball.position.y += app.ball.speed.y;

            // Collision logic: ball vs. app.screen limits
            if ((app.ball.position.x + 2 * app.ball.radius) >= Screen::GetWidth<float>()) {
                app.ball.position.x = Screen::GetWidth<float>() - 2 * app.ball.radius;
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
                PlaySound(app.GetSound("bounce"));
            }

            // Collision logic: ball vs. app.bricks
            for (int j = 0; j < BRICKS_LINES; ++j) { // NOLINT(*-loop-convert)
                for (int i = 0; i < BRICKS_PER_LINE; ++i) {
                    if (app.bricks[j][i].active) {
                        if (CheckCollisionCircleRec(ballCenter, app.ball.radius, app.bricks[j][i].bounds)) {
                            --app.bricks[j][i].resistance;
                            app.ball.speed.y *= -1;
                            if (app.bricks[j][i].resistance <= 0) {
                                app.bricks[j][i].active = false;
                                --app.numActiveBricks;
                                PlaySound(app.GetSound("explosion"));
                                if (app.numActiveBricks == 0) {
                                    app.gameResult = 1;
                                    return std::make_unique<EndingScreen>();
                                }
                            } else {
                                PlaySound(app.GetSound("bounce"));
                            }
                            return {};
                        }
                    }
                }
            }

            // Game ending logic
            if ((ballCenter.y + app.ball.radius) >= Screen::GetHeight<float>()) {
                app.ball.position.x = app.player.position.x + app.player.size.x / 2 - app.ball.radius;
                app.ball.position.y = app.player.position.y - app.ball.radius * 2;
                app.ball.active = false;
                app.player.lives--;
            }

            if (app.player.lives <= 0) {
                app.player.lives = NUM_PLAYER_LIVES;
                app.gameResult = app.numActiveBricks == 0 ? 1 : 0;
                app.framesCounter = 0;
                return std::make_unique<EndingScreen>();
            }
        } else {
            // Reset ball position
            app.ball.position.x = app.player.position.x + app.player.size.x / 2 - app.ball.radius;
            if (IsKeyPressed(KEY_SPACE)) {
                const float angle = static_cast<float>(GetRandomValue(-300, 300)) / 10.0f;
                app.ball.speed = Vector2Rotate(Vector2{0.0f, -5.0f}, angle * DEG2RAD);
                app.ball.active = true;
            }
        }
    }
    return {};
}

void GameplayScreen::DrawFrame(const App& app) const {
    for (int j = 0; j < BRICKS_LINES; ++j) {
        for (int i = 0; i < BRICKS_PER_LINE; ++i) {
            if (app.bricks[j][i].active) {
                if ((i + j) % 2 == 0) {
                    DrawTextureEx(app.GetTexture("brick"), app.bricks[j][i].position, 0.0f, app.brickScale, GRAY);
                } else {
                    DrawTextureEx(app.GetTexture("brick"), app.bricks[j][i].position, 0.0f, app.brickScale, DARKGRAY);
                }
            }
        }
    }

    // Draw player
    DrawTextureEx(app.GetTexture("paddle"), app.player.position, 0.0f, 1.0f, WHITE);
    // Draw ball
    DrawTextureEx(app.GetTexture("ball"), app.ball.position, 0.0f, 1.0f, MAROON);
    DrawGui(app);

}

std::unique_ptr<ScreenImpl> GameplayScreen::HandleDebugKeys(App& app) {
    if (IsKeyPressed('Q')) {
        app.player.lives = 0;
        app.gameResult = app.numActiveBricks == 0 ? 1 : 0;
        return std::make_unique<EndingScreen>();
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
        return std::make_unique<EndingScreen>();
    }
    if (IsKeyPressed('L')) {
        app.player.lives = 0;
        app.gameResult = 0;
        return std::make_unique<EndingScreen>();
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
                        app.gameResult = 1;
                        return std::make_unique<EndingScreen>();
                    }
                }
            }
        }
    }
    return {};
}

void GameplayScreen::DrawGui(const App& app) {// Draw GUI: player lives
    for (int i = 0; i < app.player.lives; ++i) {
        DrawRectangle(20 + 40 * i, GetScreenHeight() - 30, 35, 10, LIGHTGRAY);
    }

    // Draw pause message when required
    if (app.gamePaused) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(RAYWHITE, 0.8f));
        DrawText("GAME PAUSED", GetScreenWidth() / 2 - MeasureText("GAME PAUSED", 40) / 2,
                 GetScreenHeight() / 2 - 40, 40, GRAY);
    }
}

std::unique_ptr<ScreenImpl> EndingScreen::UpdateGameState(App& app) {

    ++app.framesCounter;

    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
        return std::make_unique<TitleScreen>();
    }
    return {};
}

void EndingScreen::DrawFrame(const App& app) const {

    if (app.gameResult == 0) {
        if (app.numActiveBricks > BRICKS_PER_LINE * BRICKS_LINES / 2) {
            Screen::DrawCenteredText(app, "setback", "YOU LOSE!", 100, 80, 5, MAROON);
        } else {
            Screen::DrawCenteredText(app, "setback", "GAME OVER!", 100, 80, 5, MAROON);
        }
    } else if (app.gameResult == 1) {
        Screen::DrawCenteredText(app, "setback", "YOU WIN!", 100, 80, 5, MAROON);
    } else {
        Screen::DrawCenteredText(app, "setback", "WHAT HAPPENED?", 100, 80, 5, MAROON);
    }

    if ((app.framesCounter / 30) % 2 == 0) {
        Screen::DrawCenteredText("PRESS [ENTER] TO PLAY AGAIN",
                                 GetScreenHeight() / 2 + 80, 20, GRAY);
    }

}
