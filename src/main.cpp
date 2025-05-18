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
texture_master texture_manager;
#include "item_master.hpp"
item_master item_manager;
#include "block_master.hpp"
block_master block_manager;
#include "world.hpp"
world_class world;
#include "render.hpp"
render render_master;

#include "rng.hpp"
#include "world.hpp"

int main() {
    InitWindow(800, 450, "Raylib Test");
    rlImGuiSetup(true);

    texture_manager.set_default_texture("reasource/gfx/default.png");
    item_manager.load_item_declaration_file("reasource/items.json");

    world_class world;

    SetTargetFPS(165);

    while (!WindowShouldClose()) {
        texture_manager.update();
        BeginDrawing();
        ClearBackground(RAYWHITE);

        render_master.update();

        if (IsKeyDown(KEY_W)) render_master.move_camera_y(-1);
        if (IsKeyDown(KEY_S)) render_master.move_camera_y(1);
        if (IsKeyDown(KEY_D)) render_master.move_camera_x(-1);
        if (IsKeyDown(KEY_A)) render_master.move_camera_x(1);

        rlImGuiBegin();
        ImGui::Text("Hello, world %d", 123);
        rlImGuiEnd();

        EndDrawing();
    }
    CloseWindow();
    rlImGuiShutdown();
    return 0;
}
