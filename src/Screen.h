#pragma once
#ifndef BLOCKS_GAME_SCREEN_H
#define BLOCKS_GAME_SCREEN_H

#include <cstdio>
#include <memory>
#include <string>
#include "raylib.h"
#include "Common.h"

class App;

class ScreenImpl {
public:
    ScreenImpl() = default;
    ScreenImpl(const ScreenImpl&) = delete;
    ScreenImpl& operator=(const ScreenImpl&) = delete;
    ScreenImpl(ScreenImpl&&) = delete;
    ScreenImpl& operator=(ScreenImpl&&) = delete;
    virtual ~ScreenImpl() = default;

    virtual std::unique_ptr<ScreenImpl> UpdateGameState(App& app) = 0;
    virtual void DrawFrame(const App& app) const = 0;
};

class LogoScreen : public ScreenImpl {
public:
    std::unique_ptr<ScreenImpl> UpdateGameState(App& app) override;
    void DrawFrame(const App& app) const override;
};

class TitleScreen : public ScreenImpl {
public:
    std::unique_ptr<ScreenImpl> UpdateGameState(App& app) override;
    void DrawFrame(const App& app) const override;
};

class GameplayScreen : public ScreenImpl {
public:
    std::unique_ptr<ScreenImpl> UpdateGameState(App& app) override;
    void DrawFrame(const App& app) const override;

private:
    static std::unique_ptr<ScreenImpl> HandleDebugKeys(App& app);
    static void DrawGui(const App& app);
};

class EndingScreen : public ScreenImpl {
public:
    std::unique_ptr<ScreenImpl> UpdateGameState(App& app) override;
    void DrawFrame(const App& app) const override;
};

class Screen {
public:
    template<typename T=int>
    static T GetHeight() {
        return static_cast<T>(GetScreenHeight());
    }

    template<typename T=int>
    static T GetWidth() {
        return static_cast<T>(GetScreenWidth());
    }

    static void DrawCenteredText(const char* text, int posY, int fontSize, Color color);
    static void DrawCenteredText(const App& app,
                                 const std::string& fontName,
                                 const char* text,
                                 int posY,
                                 int fontSize,
                                 int spacing,
                                 Color color);
    static void DrawFlashingText(const App& app, const char* text, int posY, int fontSize, Color color);

    void UpdateGameState(App& app);
    void DrawFrame(const App& app) const;
private:
    std::unique_ptr<ScreenImpl> screenImpl{std::make_unique<LogoScreen>()};
};

#endif //BLOCKS_GAME_SCREEN_H
