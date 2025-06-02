#include "inventory.hpp"
#include "item_master.hpp"
#include "networking.hpp"
#include "texture_master.hpp"
#include "player.hpp"


inventory::inventory(){
    for(int i = 0; i < MAX_INVENTORY_SIZE; i++){
        slots[i].item_i.item_id = 0;
        slots[i].count = 0;
    }
}

inventory_slot* inventory::search_inventory(item* itm){
    for(int i = 0; i < MAX_INVENTORY_SIZE; i++){
        if(slots[i].item_i.item_id == itm->item_id){
            return &slots[i];
        }
    }

    return nullptr;
}

inventory_slot* inventory::find_empty_slot(){
    for(int i = 0; i < MAX_INVENTORY_SIZE; i++){
        if(slots[i].item_i.item_id == 0){
            return &slots[i];
        }
    }

    return nullptr;
}

inventory_slot* inventory::get_slot_for_item(item* itm){
    inventory_slot* slot = search_inventory(itm);

    if(slot == nullptr){
        slot = find_empty_slot();
    }

    slot->item_i = *itm;

    return slot;
}

bool inventory::does_have_item(item* itm, char count){
    inventory_slot* find_item = search_inventory(itm);

    if(find_item == nullptr) return false;

    if(find_item->count >= count) return true;

    return false;
}

int inventory::get_player_id(){
    return player_id;
}

void inventory::set_player_id(int id){
    player_id = id;
}

bool inventory::give_item(item* itm, char count){

    std::cout << "gave item: " + itm->name << std::endl;

    if(count >= 256) return false;

    inventory_slot* add_item_slot = get_slot_for_item(itm);

    if(add_item_slot == nullptr) return false;

    add_item_slot->count = add_item_slot->count + count;

    add_item_slot->item_i = *itm;

    if(network_manager.is_host() == true && player_id != player_manager.host_id){
        std::cout << "player id is: " << std::to_string(player_id) << std::endl;


        server_utls::give_player_item(network_manager.server_obj.get_peer_by_player_id(player_id), item_manager.get_item_name_by_id(itm->item_id), count);

    }

    

    return true;
}

void inventory_ui::init(){
    ui_texture_id = texture_manager.add_texture("reasource/gfx/ui/inventory.png", true);
    overlay_id = texture_manager.add_texture("reasource/gfx/ui/overlay.png", true);

    current_item = &inventory_i->slots[0];
}

void inventory_ui::draw_inventory(){
    if(is_inventory_opened == false) return;

    Texture2D* ui_t = texture_manager.grab_texture_pointer(ui_texture_id);

    DrawTextureEx(*ui_t, ui_pos, 0, ui_scale, WHITE);

    Vector2 pos_pointer = {0, 0};

    for (int i = 0; i < MAX_INVENTORY_SIZE; i++) {
        Vector2 mouse_pos = GetMousePosition();

        Texture2D* it = texture_manager.grab_texture_pointer(inventory_i->slots[i].item_i.texture_id);

        if(inventory_i->slots[i].item_i.item_id == 0){
            it = texture_manager.grab_texture_pointer(overlay_id);
        }

        if (!it) continue;

        float box_offset = (BOX_PADDING * 2 + BOX_SIZE) * ui_scale;

        Vector2 a = {
            INV_PADDING * ui_scale + BOX_PADDING * ui_scale + ui_pos.x + pos_pointer.x * box_offset,
            INV_PADDING * ui_scale + BOX_PADDING * ui_scale + ui_pos.y + pos_pointer.y * box_offset
        };

        Vector2 b = {
            a.x + BOX_SIZE * ui_scale,
            a.y + BOX_SIZE * ui_scale
        };

        float draw_scale = (BOX_SIZE * ui_scale) / it->width;

        Color tint = (mouse_pos.x > a.x && mouse_pos.y > a.y && mouse_pos.x < b.x && mouse_pos.y < b.y) ? GRAY : WHITE;
        DrawTextureEx(*it, a, 0, draw_scale, tint);
        std::string count_str = std::to_string(inventory_i->slots[i].count);

        DrawText(count_str.c_str(), a.x, a.y, 10, BLACK);

        if(current_item == &inventory_i->slots[i]){
            DrawRectangleLines(a.x, a.y, (b.x - a.x), (b.y - a.y), BLACK);
        }


        pos_pointer.x++;
        if (pos_pointer.x >= INV_WIDTH_SLOTS) {
            pos_pointer.x = 0;
            pos_pointer.y++;
        }
    }

    
}

void inventory_ui::set_inventory_pointer(inventory* ptr){
    inventory_i = ptr;
}

inventory* inventory_ui::get_inventory_pointer(){
    return inventory_i;
}

inventory_slot* inventory_ui::get_current_item(){
    if(current_item->count < 1) current_item->item_i.item_id = 0;
    return current_item;
}

void inventory_ui::update_inv_ui_input(){


    if(IsKeyPressed(KEY_I) && is_inventory_opened == false) is_inventory_opened = true;
    else if(IsKeyPressed(KEY_I) && is_inventory_opened == true) is_inventory_opened = false;

    

    Vector2 pos_pointer = {0, 0};

    for (int i = 0; i < MAX_INVENTORY_SIZE; i++) {
        Vector2 mouse_pos = GetMousePosition();

        Texture2D* it = texture_manager.grab_texture_pointer(inventory_i->slots[i].item_i.texture_id);

        if(inventory_i->slots[i].item_i.item_id == 0){
            it = texture_manager.grab_texture_pointer(overlay_id);
        }

        if (!it) continue;

        float box_offset = (BOX_PADDING * 2 + BOX_SIZE) * ui_scale;

        Vector2 a = {
            INV_PADDING * ui_scale + BOX_PADDING * ui_scale + ui_pos.x + pos_pointer.x * box_offset,
            INV_PADDING * ui_scale + BOX_PADDING * ui_scale + ui_pos.y + pos_pointer.y * box_offset
        };

        Vector2 b = {
            a.x + BOX_SIZE * ui_scale,
            a.y + BOX_SIZE * ui_scale
        };

        if(mouse_pos.x > a.x && mouse_pos.y > a.y && mouse_pos.x < b.x && mouse_pos.y < b.y){
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                current_item = &inventory_i->slots[i];
            }
        }

        if(current_item == &inventory_i->slots[i]){
            DrawRectangleLines(a.x, a.y, (b.x - a.x), (b.y - a.y), BLACK);
        }


        pos_pointer.x++;
        if (pos_pointer.x >= INV_WIDTH_SLOTS) {
            pos_pointer.x = 0;
            pos_pointer.y++;
        }
    }
}