#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

int main() {
    InitWindow(800, 450, "Raylib Test");
    rlImGuiSetup(true);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Hello from Raylib!", 190, 200, 20, LIGHTGRAY);

        rlImGuiBegin();
        ImGui::Text("Hello, world %d", 123);
        rlImGuiEnd();

        EndDrawing();
    }
    CloseWindow();
    rlImGuiShutdown();
    return 0;
}