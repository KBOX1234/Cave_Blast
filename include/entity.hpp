#include "raylib.h"
#include <iostream>
#include <vector>

#define NO_PATHFIND 0
#define GRID_PATHFIND 1
#define DIAGONAL_PATHFIND 2

class entity_master;

class entity {
    friend class entity_master;
private:
    Vector2 pos;
    std::string perm_name;
    bool hostile;
    std::vector<std::string> hated_entitys;
    std::vector<std::string> scared_of;
    char pathfind_model;
    float aggro_range;
    int attack_damage;
    float sheild;
    float health;
    std::string nametag;

    

public:

    entity();
    ~entity();

    Vector2 get_pos();
    bool is_hostile();
    int get_attack();
    float get_health();

    std::string get_name();

    std::string get_nametag();
    void set_nametage(std::string name);

};
