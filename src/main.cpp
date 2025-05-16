#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <cmath>

#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include "json.hpp"

using json = nlohmann::json;

#include "rng.hpp"
#include "world.hpp"

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