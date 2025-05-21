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
rng_device random_num;
#include "texture_master.hpp"
texture_master texture_manager;
#include "item_master.hpp"
item_master item_manager;
#include "block_master.hpp"
block_master block_manager;
#include "world.hpp"
world_class world;
#include "player.hpp"
player_master player_manager;
#include "render.hpp"
render render_master;



#include "rng.hpp"
#include "world.hpp"

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Cave Blast");
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
        if (IsKeyDown(KEY_D)) render_master.move_camera_x(1);
        if (IsKeyDown(KEY_A)) render_master.move_camera_x(-1);

        bool my_tool_active = true;
        float my_color[4];
        rlImGuiBegin();
        ImGui::Begin("Items/Blocks list", &my_tool_active, ImGuiWindowFlags_MenuBar);
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Reload Items", "Ctrl+R")) {
                    item_manager.load_item_declaration_file("reasource/items.json");
                }

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }


        ImGui::BeginChild("Scrolling");

        std::vector<std::string> entries = item_manager.get_existing_items();

        for (int n = 0; n < entries.size(); n++) {
            item* itm = item_manager.fetch_item(entries[n]);

            ImGui::Text("Item Entry %d",  n);
            ImGui::Text("   Name: %s",  itm->name.c_str());
            ImGui::Text("   Description: %s",  itm->description.c_str());
            ImGui::Text("   Item texture path: %s",  itm->texture_path.c_str());

            ImGui::Text("   Item id: %d",  itm->item_id);
            ImGui::Text("   Item texture id: %d",  itm->texture_id);

            ImGui::Text("   Item texture:");
            rlImGuiImage((const Texture*)texture_manager.grab_texture_pointer(itm->texture_id));

            if (itm->is_block) {
                ImGui::Text("   Block texture id: %d",  itm->block_type_ptr->texture_id);

                ImGui::Text("   Block type: %d",   itm->block_type_ptr->type);

                ImGui::Text("   Block texture:");
                rlImGuiImage((const Texture*)texture_manager.grab_texture_pointer(itm->block_type_ptr->texture_id));

            }





        }

        ImGui::EndChild();
        ImGui::End();
        rlImGuiEnd();


        EndDrawing();
    }
    CloseWindow();
    rlImGuiShutdown();
    return 0;
}
