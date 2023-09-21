/***********************************************************************************
 * Example based on the raylib-intro-course, with support for emscripten.
 * CMake support is based on the CMakeLists.txt file from the raylib project.
 *
 * See <https://github.com/raysan5/raylib-intro-course> for the tutorial.
 */

#include <cstdio>
#include "raylib.h"
#include "raymath.h"
#include "blocks_game.h"

int main() {
    App app;
    app.InitAppAndRunGameLoop();
    return 0;
}
