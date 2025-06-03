#pragma once
#include "raylib.h"
#include "block_master.hpp"
#include "item_master.hpp"

struct inventory_slot{
    item item_i;
    long count;
};

#define MAX_INVENTORY_SIZE 30
#define INV_PADDING 4
#define BOX_PADDING 3
#define BOX_SIZE 32

#define INV_WIDTH_SLOTS 10


class inventory_ui;


class inventory{
    friend class inventory_ui;

    private:
        inventory_slot slots[MAX_INVENTORY_SIZE];

        inventory_slot* search_inventory(item* itm);
        inventory_slot* find_empty_slot();
        inventory_slot* get_slot_for_item(item* itm);

        int player_id = 0;

    public:

        int get_player_id();

        void set_player_id(int id);

        bool give_item(item* itm, char count);

        bool take_item(item* itm, char count);

        bool does_have_item(item* i, char count);

        inventory();

};

class inventory_ui{
    private:
        inventory* inventory_i;

        Vector2 ui_pos = {20, 20};
        float ui_scale = 2;

        int ui_texture_id;


        int current_item_index = 0;

        bool is_inventory_opened = false;

        int overlay_id;

    public:

        inventory_slot* current_item;
    

        void init();

        void draw_inventory();

        void set_inventory_pointer(inventory* inv);

        inventory* get_inventory_pointer();

        inventory_slot* get_current_item();

        void update_inv_ui_input();

};