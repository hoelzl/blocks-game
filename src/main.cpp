/***********************************************************************************
 * Example based on the raylib-intro-course, with support for emscripten.
 * CMake support is based on the CMakeLists.txt file from the raylib project.
 *
 * See <https://github.com/raysan5/raylib-intro-course> for the tutorial.
 */

#include "App.h"

int main() {
    BlocksGame app("PROJECT: BLOCKS GAME", 800, 450);
    app.InitAppAndRunGameLoop();
    return 0;
}
