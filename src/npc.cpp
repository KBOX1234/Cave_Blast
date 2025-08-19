#include "npc.hpp"

npc_object::npc_object(){
    animation_manager.link_ams_linker(&amsl);
    animation_manager.link_amt_linker(&amtl);

    pos = {0, 0};

    scale = 1;

    rotation = 0;

    amtl.link_pointers(&scale, &rotation, &pos);

    amsl.set_pos(&pos);
}

void npc_object::update_colide_box(){
    
    box.a.x = pos.x;
    box.a.y = pos.y;

    Vector2 scaled_size = {size.x * scale, size.y* scale};

    box.b.x = pos.x + scaled_size.x;
    box.b.y = pos.y + scaled_size.y;
}

bool npc_object::does_npc_colide_with(colideBox* other_box){
    char result = doesBoxAndBoxColide(other_box, &box);

    return result;
}

npc::npc(){
    npc_data = new npc_object;

    pos = &npc_data->pos;
    rotation = &npc_data->rotation;
    scale = &npc_data->scale;
    size = &npc_data->size;
}

std::vector<int> npc_template_loader::find_duplicate_named_templates(std::string name){
    std::vector<int> rtv;

    for(int i = 0; i < templates.size(); i++){
        if(templates[i].name == name){
            rtv.push_back(i);
        }
    }

    return rtv;
}

bool npc_template_loader::load_template(npc_template npct, bool remove_duplicates){
    
    std::vector<int> duplicates = find_duplicate_named_templates(npct.name);
        
    if(duplicates.size() > 0){
        if(remove_duplicates == false){
            return false;
        }

        else{
            for(int i = 0; i < duplicates.size(); i++){
                int j = duplicates[i];

                templates.erase(templates.begin() + j);
            }
        }
    }
    
    templates.push_back(npct);
    
}

npc_template* npc_template_loader::get_npc_template(std::string name){
    for(int i = 0; i < templates.size(); i++){
        if(name == templates[i].name){
            return &templates[i];
        }
    }

    return nullptr;
}


