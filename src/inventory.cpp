#include "inventory.hpp"

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

bool inventory::give_item(item* itm, char count){

    if(count >= 256) return false;

    inventory_slot* add_item_slot = get_slot_for_item(itm);

    if(add_item_slot == nullptr) return false;

    if(add_item_slot->count + count < 256){
        add_item_slot->count = add_item_slot->count + count;
    }

    else{
        int items_sum = add_item_slot->count + count;

        int remainder = items_sum - 255;

        add_item_slot->count = 255;

        add_item_slot = get_slot_for_item(itm);

        add_item_slot->count = remainder;
    }

    return true;
}
