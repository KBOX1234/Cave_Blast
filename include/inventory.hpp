#include "item_master.hpp"

struct inventory_slot{
    item item_i;
    char count;
};

#define MAX_INVENTORY_SIZE 30

class inventory_ui;

class inventory{
    friend class inventory_ui;

    private:
        inventory_slot slots[MAX_INVENTORY_SIZE];

        inventory_slot* search_inventory(item* itm);
        inventory_slot* find_empty_slot();
        inventory_slot* get_slot_for_item(item* itm);

    public:

        bool give_item(item* itm, char count);

        bool take_item(item* itm, char count);

        bool does_have_item(item* i, char count);

        inventory();

};