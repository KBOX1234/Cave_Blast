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

#include "texture_master.hpp"
extern texture_master texture_manager;
#include "item_master.hpp"
item_master item_manager;

#include "rng.hpp"
#include "world.hpp"

int main() {
    InitWindow(800, 450, "Raylib Test");
    rlImGuiSetup(true);

    texture_manager.set_default_texture("reasource/gfx/default.png");
    item_manager.load_item_declaration_file("reasource/items.json");

    world_class world;

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