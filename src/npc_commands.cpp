#include "../external/raylib/src/raylib.h"

#include "../external/raylib/src/raymath.h"

#include "../include/npc.hpp"
#include "../external/sphysics/sphysics.h"
#include "../include/colide.hpp"

void npc::set_rotation(float rot) {
    *rotation = rot;
}

void npc::add_rotation(float rot) {
    *rotation = *rotation + rot;
}

void npc::move() {
    Vector2 next = get_move_target();

    if (is_valid_move_2(next)) {
        last_pos = *pos;
        *pos = next;


        //update_time_stamp();

    } else {

    }
}

bool npc::is_valid_move_2(Vector2 pos2) {

    const float box_width = size->x * *scale;
    const float box_height = size->y * *scale;

    colideBox test_box;
    test_box.a = colide::v2p(pos2);
    //test_box.a.x = test_box.a.x - box_width;


    Vector2 bottom_right = { pos2.x + box_width, pos2.y + box_height};
    test_box.b = colide::v2p(bottom_right);


    int start_block_x = (int)(pos2.x / BLOCK_SIZE) - 1;
    int start_block_y = (int)(pos2.y / BLOCK_SIZE) - 1;
    int end_block_x = (int)((pos2.x + box_width) / BLOCK_SIZE) + 1;
    int end_block_y = (int)((pos2.y + box_height) / BLOCK_SIZE) + 1;


    for (int by = start_block_y; by <= end_block_y; ++by) {
        for (int bx = start_block_x; bx <= end_block_x; ++bx) {
            Vector2 block_vec = { (float)bx - 1, (float)by};
            block* blk = world.get_block(block_vec);

            if (blk != nullptr) {
                auto item = item_manager.fetch_item_by_id(blk->attr->item_id);
                if (item->is_block == false || item->is_solid == false) continue;


                Vector2 block_top_left = { (float)(bx * BLOCK_SIZE), (float)(by * BLOCK_SIZE) };

                Vector2 block_bottom_right = {
                    block_top_left.x + BLOCK_SIZE,
                    block_top_left.y + BLOCK_SIZE
                };

                colideBox block_box;
                block_box.a = colide::v2p(block_top_left);
                block_box.b = colide::v2p(block_bottom_right);

                // 4. Check for collision
                if (doesBoxAndBoxColide(&test_box, &block_box)) {
                    return false;
                }
            }
        }
    }

    return true; // No collisions
}

Vector2 npc::get_move_target() {

    float speed = stat.speed;
    Vector2 d;
    float rad = *rotation * (M_PI / 180.0f);
    d.x = std::cos(rad) * speed;
    d.y = std::sin(rad) * speed;

    return { pos->x + d.x, pos->y + d.y };
}

float npc::distance_to_player() {
    Vector2 me = *pos;

    me.x = me.x + (size->x / 2);
    me.y = me.y + (size->y / 2);

    Vector2 player = player_manager.myself->get_pos();

    player.x = player.x + (PLAYER_WIDTH / 2);
    player.y = player.y + (PLAYER_HEIGHT / 2);

    return Vector2Distance(me, player);
}
void npc::face_player() {
    Vector2 object = *pos;
    player* p = closest_player();

    Vector2 target = p->get_pos();

    target.x = target.x + (PLAYER_WIDTH / 2);
    target.y = target.y + (PLAYER_HEIGHT / 2);


    float dx = target.x - object.x;
    float dy = target.y - object.y;

    float angleRad = std::atan2(dy, dx);

    float angleDeg = angleRad * 180.0f / M_PI;

    *rotation = angleDeg;
}

