#include <raylib-cpp.hpp>

int main() {

    const int width = 800;
    const int height = 600;

    InitWindow(width, height, "Game");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Hello world!", 100, 100, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
