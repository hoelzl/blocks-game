#include "App.h"
#include "Screen.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

App::App(const char* title, int screenWidth, int screenHeight) {
    InitWindow(screenWidth, screenHeight, title);
    InitAudioDevice();
}

App::~App() {
    CloseAudioDevice();
    CloseWindow();
}

void App::InitAppAndRunGameLoop() {
    SetMasterVolume(MASTER_VOLUME);
    PlayMusicStream(GetMusic("go-wild"));

    InitializePlayer();
    InitializeBall();
    InitializeBricks();

    RunGameLoop();
}

void App::RunGameLoop() {
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

void App::InitializeBricks() {// Initialize bricks
    brickScale = Screen::GetWidth<float>() / ((float) BRICKS_PER_LINE * (float) GetTexture("brick").width);
    for (int j = 0; j < BRICKS_LINES; ++j) {
        for (int i = 0; i < BRICKS_PER_LINE; ++i) {
            bricks[j][i].size = Vector2{(float) GetTexture("brick").width * brickScale,
                                        (float) GetTexture("brick").height * brickScale};
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

void App::InitializeBall() {// Initialize ball
    ball.radius = 10.0f;
    ball.active = false;
    ball.position = Vector2{player.position.x + player.size.x / 2 - ball.radius,
                            player.position.y - ball.radius * 2};
    ball.speed = Vector2{0.0f, 0.0f};
}

void App::InitializePlayer() {// Initialize player
    player.position = Vector2{Screen::GetWidth<float>() / 2.0f, Screen::GetHeight<float>() * 7.0f / 8.0f};
    player.speed = Vector2{8.0f, 8.0f};
    player.size = Vector2{100, 24};
    player.lives = NUM_PLAYER_LIVES;
}

void App::UpdateGameStateAndDrawFrame() {
    screen.UpdateGameState(*this);
    UpdateMusicStream(GetMusic("go-wild"));
    screen.DrawFrame(*this);
}

BlocksGame::BlocksGame(const char* title, int screenWidth, int screenHeight) : App(title, screenWidth, screenHeight) {
    LoadTexture("logo", "resources/raylib_logo.png");
    LoadTexture("ball", "resources/ball.png");
    LoadTexture("paddle", "resources/paddle.png");
    LoadTexture("brick", "resources/brick.png");

    LoadFont("setback", "resources/setback.png");

    LoadSound("start", "resources/start.wav");
    LoadSound("bounce", "resources/bounce.wav");
    LoadSound("explosion", "resources/explosion.wav");

    SetSoundVolume(GetSound("start"), SOUND_FX_VOLUME);
    SetSoundVolume(GetSound("bounce"), SOUND_FX_VOLUME);
    SetSoundVolume(GetSound("explosion"), SOUND_FX_VOLUME);

    LoadMusic("go-wild", "resources/go-wild.mp3");
    SetMusicVolume(GetMusic("go-wild"), MUSIC_STREAM_VOLUME);
}
