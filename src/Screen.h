#pragma once
#ifndef BLOCKS_GAME_SCREEN_H
#define BLOCKS_GAME_SCREEN_H

#include <cstdio>
#include <memory>
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
public:
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
};

class EndingScreen : public ScreenImpl {
public:
    std::unique_ptr<ScreenImpl> UpdateGameState(App& app) override;
    void DrawFrame(const App& app) const override;
};

class Screen {
public:
    static float GetScreenHeightFloat() {
        return static_cast<float>(GetScreenHeight());
    }

    static float GetScreenWidthFloat() {
        return static_cast<float>(GetScreenWidth());
    }

    void UpdateGameState(App& app);
    void DrawFrame(const App& app) const;

private:
    std::unique_ptr<ScreenImpl> screenImpl{std::make_unique<LogoScreen>()};
};

#endif //BLOCKS_GAME_SCREEN_H
