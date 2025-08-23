#include "npc.hpp"
#include "texture_master.hpp"
#include "json.hpp"
#include "io.hpp"
using json = nlohmann::json;
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

void npc::draw(){
    
    if(cache == nullptr){
        cache = texture_manager.grab_texture_pointer(texture_id);

        if(cache == nullptr){
           // printf("NULLLLLLLLL\n");
        }
    }

    if(cache != nullptr && pos != nullptr && rotation !=  nullptr && scale != nullptr){
    
        //printf("debug:\npos = {%f, %f}\nrotation = %f\nscale = %f\n", pos->x, pos->y, *rotation, *scale);

        DrawTextureEx(*cache, *pos, *rotation, *scale, WHITE);
    }
    
}

void npc::assign_my_pointer(void* ptr){
    myself = ptr;
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

    return true;
    
}

npc_template* npc_template_loader::get_npc_template(std::string name){
    for(int i = 0; i < templates.size(); i++){
        if(name == templates[i].name){
            return &templates[i];
        }
    }

    return nullptr;
}

bool npc_template_loader::load_templates_from_json(std::string fname){
    json j_list = json::parse(easy_file_ops::load_text_file(fname));

    for(int i = 0; i < j_list.size(); i++){
        json j_object = j_list[i];

        npc_template nt;

        nt.name = j_object.value("name", "no_name");
        Vector2 size;

        if(j_object.contains("size")){
            size.x = j_object["size"]["x"];
            size.y = j_object["size"]["y"];
        }
        else{
            size = {0, 0};
        }

        nt.stat.max_health = j_object.value("max_health", 1);
        nt.stat.max_damage = j_object.value("damage_output", 1);
        nt.stat.max_block = j_object.value("max_block", 1);
        nt.stat.speed = j_object.value("speed", 0);
        nt.stat.target_player = j_object.value("target_player", false);


        nt.texture_id = texture_manager.add_texture(j_object.value("texture", " "));

        load_template(nt, true);
    }

    return true;
}
