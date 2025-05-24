#include "../include/networking.hpp"

#include <iostream>

#include "../include/player.hpp"

void network::update_client() {

    if (player_manager.get_host() != nullptr && input_manager.is_there_input_update() == true) client_utls::move_myself(player_manager.get_host()->get_rotation());
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

                    std::cout << "set block\n";
                }

                enet_packet_destroy(event.packet);
                break;
            } // End of scope for RECEIVE case

            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Server Disconnected\n";
                break;
        }

    }
}

void client_utls::fetch_player(int id) {
    packet* send_p = new packet;

    send_p->type = GET_PLAYER;
    send_p->size = sizeof(int);

    send_p->data = new char[sizeof(int)];

    memcpy(send_p->data, &id, sizeof(int));

    char* buffer = net_utills::convert_to_buffer(send_p);

    net_utills::send_msg_safe(buffer, net_utills::get_packet_size(send_p), networking.remote_instance, 0);
}

void client_utls::send_player_list_request() {
    packet* p = new packet;

    p->type = GET_PLAYER_LIST;
    p->size = 0;
    p->data = nullptr;

    char* buffer = net_utills::convert_to_buffer(p);

    net_utills::send_msg_safe(buffer, net_utills::get_packet_size(p), networking.remote_instance, 0);
}


void client_utls::move_myself(float angle) {
    packet* p = new packet;

    p->type = MOVE;
    p->size = sizeof(float);

    p->data = new char[p->size];

    memcpy(p->data, &angle, p->size);

    char* buff = net_utills::convert_to_buffer(p);

    if (buff == nullptr) {
        std::cout << "\nissue packing packet\n";
        return;
    }

    net_utills::send_msg_safe(buff, net_utills::get_packet_size(p), networking.remote_instance, 0);
    //delete[] static_cast<char*>(p->data);

    //delete p;
}

void client_utls::player_creation_request(std::string name) {
    packet* p = new packet;

    p->type = CREATE_PLAYER;

    const char* new_str = name.c_str();
    p->size = strlen(new_str) + 1;

    p->data = new char[p->size];


    printf("requesting new player: %s", new_str);
    memcpy(p->data, new_str, p->size);

    char* buff = net_utills::convert_to_buffer(p);

    if (buff == nullptr) {
        std::cout << "\nissue packing packet\n";
        return;
    }

    net_utills::send_msg_safe(buff, net_utills::get_packet_size(p), networking.remote_instance, 0);

    //delete[] static_cast<char*>(p->data);
    //delete p;
}

void client_utls::place_block(std::string name, Vector2 pos) {
    if (networking.is_server == false) {
        size_t data_size = strlen(name.c_str()) + 1;

        data_size = data_size + sizeof(Vector2);

        packet* send_p = new packet;

        send_p->type = SET_BLOCK;

        send_p->size = data_size;

        char* data_b = new char[data_size];

        size_t point = sizeof(Vector2);

        memcpy(data_b, &pos, point);

        memcpy(data_b + point, name.c_str(), strlen(name.c_str()) + 1);

        send_p->data = data_b;

        char* buffer = net_utills::convert_to_buffer(send_p);

        net_utills::send_msg_safe(buffer, net_utills::get_packet_size(send_p), networking.remote_instance, 0);
    }
}
