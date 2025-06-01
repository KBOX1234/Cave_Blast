#include "../include/networking.hpp"

#include <iostream>

#include "../include/player.hpp"

void network::update_client() {

    if (player_manager.get_host() != nullptr) client_utls::move_myself(player_manager.get_host()->get_rotation(), player_manager.get_host()->get_pos());
    //client_utls::fetch_all_players();
    client_utls::send_player_list_request();

    ENetEvent event;
    /* Wait up to 0 milliseconds for an event. */
    while (enet_host_service(local_instance, &event, 0) > 0)
    {


        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
                break;

            case ENET_EVENT_TYPE_RECEIVE:
            {
                // Begin a new scope here
                packet* p = net_utills::convert_from_buffer((char*)event.packet->data, event.packet->dataLength);


                if (player_manager.get_host() != nullptr) {
                    //std::cout << "my id is: " + std::to_string(player_manager.host_id) + "\n";
                    //std::cout << "my official id is: " + std::to_string(player_manager.get_host()->get_id()) << std::endl;
                    //std::cout << "my pos is: " + std::to_string(player_manager.get_host()->get_pos().x) + ", " + std::to_string(player_manager.get_host()->get_pos().x) << std::endl;
                    if (p->type == GET_PLAYER_LIST) {
                        std::vector<int> ids;

                        ids = deserializeIntVector(p->data, p->size);

                        for (int i = 0; i < ids.size(); i++) {
                            //std::cout << "id: " << std::to_string(ids[i]) << std::endl;

                            client_utls::fetch_player(ids[i]);
                        }
                    }

                    if (p->type == GET_PLAYER) {
                        serialized_player spl;

                        memcpy(&spl, p->data, sizeof(serialized_player));

                        if (player_manager.does_player_exist(spl.id) == false) {
                            player_manager.add_player_from_serialised_player(&spl);
                        }

                        else {
                            player* pl = player_manager.fetch_player_data(spl.id);
                            if (pl != nullptr) {
                                pl->set_pos(spl.pos);
                                pl->set_stats(spl.stats);

                            }
                            else {
                                std::cout << "player id: " + std::to_string(spl.id) + " does not exist\n";
                            }
                        }
                    }
                }

                if (p->type == CREATE_PLAYER) {

                    serialized_player splr;

                    std::cout << "got player creation response from server.\nplayer id: " << std::to_string(splr.id) << std::endl;

                    memcpy(&splr, p->data, sizeof(serialized_player));

                    player_manager.add_player_from_serialised_player(&splr);

                    player_manager.host_id = splr.id;

                    player_manager.myself->set_id(splr.id);

                    if (player_manager.re_sync_timer < 2 * GetFPS()) {
                        player_manager.re_sync_timer ++;
                    }
                    else {
                        player_manager.myself->set_pos(player_manager.get_host()->get_pos());
                        player_manager.myself->zero_rotation();
                        player_manager.myself->increase_angle(player_manager.get_host()->get_rotation());
                    }

                }

                if (p->type == SET_BLOCK) {
                    char* name = new char[p->size - sizeof(Vector2)];

                    Vector2 pos;

                    memcpy(&pos, p->data, sizeof(Vector2));

                    memcpy(name, p->data + sizeof(Vector2), p->size - sizeof(Vector2));

                    block blk;

                    blk.state = 0;

                    blk.attr = item_manager.fetch_item(name)->block_type_ptr;

                    world.place_block(pos, blk);

                    //std::cout << "set block\n";
                }

                if (p->type == DISCONNECT_PLAYER) {
                    int id;

                    memcpy(&id, p->data, sizeof(int));

                    player_manager.remove_player(id);
                }

                if (p->type == RE_CALIBRATE) {
                    Vector2 n_pos;

                    memcpy(&n_pos, p->data, sizeof(Vector2));

                    player_manager.get_host()->set_pos(n_pos);
                }

                if (p->type == GET_ALL_PLAYERS) {
                    size_t size = (size_t)round(p->size / sizeof(serialized_player));
                    size_t inc = 0;



                    while (inc < p->size) {
                        serialized_player spl;

                        memcpy(&spl, p->data + inc, sizeof(serialized_player));

                        inc = inc + sizeof(serialized_player);

                        player* player_p = player_manager.fetch_player_data(spl.id);

                        if (player_p == nullptr) {
                            player_p = player_manager.fetch_player_data(player_manager.add_player_from_serialised_player(&spl));
                        }
                        else {
                            player_p->set_name(spl.name);
                            player_p->set_pos(spl.pos);
                            player_p->set_stats(spl.stats);

                            player_p->zero_rotation(true);
                            player_p->increase_angle(spl.angle, true);
                        }
                    }


                }

                enet_packet_destroy(event.packet);
                break;
            } // End of scope for RECEIVE case

            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Server Disconnected\n";
                exit(-2);
                break;
        }

    }
}