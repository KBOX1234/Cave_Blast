#include "imgui_window.hpp"

#include "crafting.hpp"

void imgui_win::items_menu() {
    ImGui::Begin("Items/Blocks list", &items_menu_active, ImGuiWindowFlags_MenuBar);
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
}

void imgui_win::player_data_menu() {
    ImGui::Begin("Player Data", &items_menu_active, ImGuiWindowFlags_MenuBar);
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

    //ImGui::Text("Player pos is: %f, %f", player_manager.host->get_pos().x, player_manager.host->get_pos().y);
    ImGui::Text("Player pos is: %f, %f", player_manager.get_host()->get_pos().x / BLOCK_SIZE, player_manager.get_host()->get_pos().y / BLOCK_SIZE);
    ImGui::Text("Cursor pos is: %f, %f", input_manager.cursor.x, input_manager.cursor.y);

    ImGui::EndChild();
    ImGui::End();
}

void imgui_win::craft_view_menue() {
    ImGui::Begin("Crafter Recipes", &items_menu_active, ImGuiWindowFlags_MenuBar);
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

    std::vector<std::string> rcps = crafting_manager.get_avalible_crafting_recipies(player_manager.myself);

    for (int i = 0; i < rcps.size(); i++) {
        item* itm = item_manager.fetch_item(rcps[i]);

        Texture2D* txt = texture_manager.grab_texture_pointer(itm->texture_id);

        if (rlImGuiImageButtonSize(itm->name.c_str(), txt, {64, 64})) {
            crafting_manager.craft_item(i, player_manager.myself);
        }
    }

    ImGui::EndChild();
    ImGui::End();
}
