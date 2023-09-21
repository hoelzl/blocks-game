#include "App.h"
#include "Screen.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

void App::InitAppAndRunGameLoop() {
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

void App::UnloadSoundResources() const {
    UnloadSound(fxStart);
    UnloadSound(fxBounce);
    UnloadSound(fxExplode);

    UnloadMusicStream(music);
}

void App::UnloadGraphicsResources() const {// Unload textures
    UnloadTexture(texBall);
    UnloadTexture(texPaddle);
    UnloadTexture(texBrick);

    UnloadFont(font);
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
    brickScale = Screen::GetWidth<float>() / ((float) BRICKS_PER_LINE * (float) texBrick.width);
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

void App::LoadSoundResources() {
    fxStart = LoadSound("resources/start.wav");
    fxBounce = LoadSound("resources/bounce.wav");
    fxExplode = LoadSound("resources/explosion.wav");

    SetSoundVolume(fxStart, SOUND_FX_VOLUME);
    SetSoundVolume(fxBounce, SOUND_FX_VOLUME);
    SetSoundVolume(fxExplode, SOUND_FX_VOLUME);

    music = LoadMusicStream("resources/go-wild.mp3");

    SetMusicVolume(music, MUSIC_STREAM_VOLUME);
}

void App::LoadGraphicsResources() {// Note: Load resources after window initialization (OpenGL context is required)
    texLogo = LoadTexture("resources/raylib_logo.png");
    texBall = LoadTexture("resources/ball.png");
    texPaddle = LoadTexture("resources/paddle.png");
    texBrick = LoadTexture("resources/brick.png");

    font = LoadFont("resources/setback.png");
}

void App::UpdateGameStateAndDrawFrame() {
    screen.UpdateGameState(*this);
    UpdateMusicStream(music);
    screen.DrawFrame(*this);
}
