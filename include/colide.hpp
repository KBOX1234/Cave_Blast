#pragma once
#include "player.hpp"
#include "world.hpp"

class colide{

    private:

        player_master* player_base;

        world_class* world_base;

        bool check_player(player* p);

    public:

        static point v2p(Vector2 v);

        static Vector2 p2v(point p);

        void set_player_base(player_master* pb);

        void set_world_base(world_class* wb);

        void check_player_base();
};