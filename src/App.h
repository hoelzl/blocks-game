#pragma once
#ifndef BLOCKS_GAME_APP_H
#define BLOCKS_GAME_APP_H

#include "Common.h"
#include "Actors.h"
#include "Screen.h"
#include <memory>
#include <string>
#include <unordered_map>

template<typename T, T(* Load)(const char*), void(* Release)(T)>
class RaiiWrapper {
public:
    RaiiWrapper() = default;

    [[maybe_unused]] explicit RaiiWrapper(const char* fileName) {
        resource = Load(fileName);
    }

    RaiiWrapper(const RaiiWrapper&) = delete;
    RaiiWrapper& operator=(const RaiiWrapper&) = delete;

    RaiiWrapper(RaiiWrapper&& other) noexcept {
        std::swap(resource, other.resource);
    }

    RaiiWrapper& operator=(RaiiWrapper&& other) noexcept {
        std::swap(resource, other.resource);
        return *this;
    }

    ~RaiiWrapper() {
        Release(resource);
    }

    [[nodiscard]] T GetResource() const {
        return resource;
    }

private:
    T resource{};
};

using RaiiTexture2D = RaiiWrapper<Texture2D, LoadTexture, UnloadTexture>;
using RaiiFont = RaiiWrapper<Font, LoadFont, UnloadFont>;
using RaiiSound = RaiiWrapper<Sound, LoadSound, UnloadSound>;
using RaiiMusic = RaiiWrapper<Music, LoadMusicStream, UnloadMusicStream>;

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

    bool HasTexture(const std::string& textureName) const {
        return textures.find(textureName) != textures.end();
    }

    [[nodiscard]] Texture2D GetTexture(const std::string& textureName) const {
        if (!HasTexture(textureName)) {
            TraceLog(LOG_WARNING, "Texture %s not found", textureName.c_str());
            return {};
        }
        return textures.at(textureName).GetResource();
    }

    void LoadTexture(const std::string& textureName, const char* fileName) {
        textures.emplace(textureName, RaiiTexture2D{fileName});
    }

    [[nodiscard]] bool HasFont(const std::string& fontName) const {
        return fonts.find(fontName) != fonts.end();
    }

    [[nodiscard]] Font GetFont(const std::string& fontName) const {
        if (!HasFont(fontName)) {
            TraceLog(LOG_WARNING, "Font %s not found", fontName.c_str());
            return {};
        }
        return fonts.at(fontName).GetResource();
    }

    void LoadFont(const std::string& fontName, const char* fileName) {
        fonts.emplace(fontName, RaiiFont(fileName));
    }

    [[nodiscard]] bool HasSound(const std::string& soundName) const {
        return sounds.find(soundName) != sounds.end();
    }

    [[nodiscard]] Sound GetSound(const std::string& soundName) const {
        if (!HasSound(soundName)) {
            TraceLog(LOG_WARNING, "Sound %s not found", soundName.c_str());
            return {};
        }
        return sounds.at(soundName).GetResource();
    }

    void LoadSound(const std::string& soundName, const char* fileName) {
        sounds.emplace(soundName, RaiiSound(fileName));
    }

    [[nodiscard]] bool HasMusic(const std::string& musicName) const {
        return music.find(musicName) != music.end();
    }

    Music GetMusic(const std::string& musicName) const {
        if (!HasMusic(musicName)) {
            TraceLog(LOG_WARNING, "Music %s not found", musicName.c_str());
            return {};
        }
        return music.at(musicName).GetResource();
    }

    void LoadMusic(const std::string& musicName, const char* fileName) {
        music.emplace(musicName, RaiiMusic(fileName));
    }

    int framesCounter{0};
    int gameResult{-1};     // -1 = Game not finished, 1 = Win, 0 = Lose
    bool gamePaused{false};

    void RunGameLoop();
    void InitializeBricks();
    void InitializeBall();
    void InitializePlayer();
    void UpdateGameStateAndDrawFrame();

private:
    std::unordered_map<std::string, RaiiTexture2D> textures{};
    std::unordered_map<std::string, RaiiFont> fonts{};
    std::unordered_map<std::string, RaiiSound> sounds{};
    std::unordered_map<std::string, RaiiMusic> music{};
};

class BlocksGame : public App {
public:
    BlocksGame(const char* title, int screenWidth, int screenHeight);
};

#endif //BLOCKS_GAME_APP_H
