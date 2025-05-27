#include "../include/networking.hpp"

#include <iostream>

#include "../include/player.hpp"

void network::handle_connect(ENetEvent *event) {
    printf("A new client connected from %x:%u.\n",
    event->peer->address.host,
    event->peer->address.port);

    clients.push_back(event->peer);
}


void network::handle_request(ENetEvent* event) {
    ENetPacket* epacket = event->packet;
    char* buffer = reinterpret_cast<char*>(epacket->data);
    size_t buffer_size = epacket->dataLength;

    packet* p = net_utills::convert_from_buffer(buffer, buffer_size);
    if (!p) {
        std::cerr << "Failed to deserialize packet" << std::endl;
        return;
    }

    if (p->type == CREATE_PLAYER) server_utls::handle_player_creation(event, p);
    else if (p->type == MOVE) server_utls::handle_move_packet(event, p);
    else if (p->type == GET_PLAYER_LIST) server_utls::send_player_list(event, p);
    else if (p->type == GET_PLAYER) server_utls::send_player_data(event, p);
    else if (p->type == SET_BLOCK) server_utls::handle_player_block_placement(event, p);


    delete[] p->data;
    delete p;
}



void network::handle_disconnect(ENetEvent *event) {
    int id;

    memcpy(&id, event->peer->data, sizeof(int));

    std::cout <<  "client disconnected: "<< std::to_string(id) << std::endl;

    player_manager.remove_player(id);
    send_p_connection_loss(event);
}


void network::update_server() {
    ENetEvent event = {};

    while (enet_host_service(local_instance, &event, 0) > 0)
    {
        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                handle_connect(&event);
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {
                handle_request(&event);
                enet_packet_destroy(event.packet);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                handle_disconnect(&event);

                break;
            }

        }

    }

    for (size_t i = 0; i < local_instance->peerCount; ++i) {
        ENetPeer* peer = &local_instance->peers[i];

        if (peer->state == ENET_PEER_STATE_CONNECTED) {
            send_block_changes(peer);
        }
    }
}

void network::add_block_change(block_change blk_chng) {
    blk_change.push_back(blk_chng);
}

void network::send_block_changes(ENetPeer *to) {
    for (int i = 0; i < blk_change.size(); i++) {
        packet p;

        p.type = SET_BLOCK;

        size_t total_d_size = strlen(blk_change[i].blk_name.c_str()) + 1 + sizeof(Vector2);

        char* data_b = new char[total_d_size];

        memcpy(data_b, &blk_change[i].pos, sizeof(Vector2));
        memcpy(data_b + sizeof(Vector2), blk_change[i].blk_name.c_str(), strlen(blk_change[i].blk_name.c_str()) + 1);

        p.size = total_d_size;
        p.data = data_b;

        char* buffer = net_utills::convert_to_buffer(&p);

        send_msg_safe(buffer,  net_utills::get_packet_size(&p), to, 0);

    }
    blk_change.clear();

}

void network::send_p_connection_loss(ENetEvent *event) {
    packet pp;

    pp.type = DISCONNECT_PLAYER;

    pp.size = sizeof(int);

    pp.data = (char*)event->peer->data;

    char* buffer = net_utills::convert_to_buffer(&pp);

    for (int i = 0; i < clients.size(); i++) {
        send_msg_safe(buffer, net_utills::get_packet_size(&pp), clients[i], 0);
    }

}

void server_utls::handle_player_creation(ENetEvent *event, packet* p) {
    std::string new_name(p->data, p->size);
    int p_id = player_manager.add_player(new_name);

    auto* p_id_ptr = new int(p_id);
    event->peer->data = p_id_ptr;

    player* plr = player_manager.fetch_player_data(p_id);

    serialized_player splr = plr->serialize();

    packet send_p;
    send_p.type = CREATE_PLAYER;
    send_p.size = sizeof(serialized_player);
    send_p.data = (char*)&splr;

    char* buffer = net_utills::convert_to_buffer(&send_p);
    net_utills::send_msg_safe(buffer, net_utills::get_packet_size(&send_p), event->peer, 0);

    delete[] buffer;
}

void server_utls::handle_move_packet(ENetEvent *event, packet *p) {
    int* id_ptr = static_cast<int*>(event->peer->data);
    if (id_ptr && p->size >= sizeof(float) + sizeof(Vector2)) {
        int id = *id_ptr;
        Vector2 pos;
        float angle;
        std::memcpy(&pos, p->data, sizeof(Vector2));
        memcpy(&angle, p->data + sizeof(Vector2), sizeof(float));

        player_manager.players[id]->zero_rotation();
        player_manager.players[id]->increase_angle(angle);

        packet back_p;

        back_p.type = RE_CALIBRATE;
        back_p.size = sizeof(Vector2);

        //does not want to work so for now we will take client coords at face value
        //if (player_manager.players[id]->is_valid_move(pos) == true) {
        if (true) {
            player_manager.players[id]->set_pos(pos);
            player_manager.players[id]->update_time_stamp();
            back_p.data = (char*)&pos;


        }
        else {
            pos = player_manager.players[id]->get_pos();
            back_p.data = (char*)&pos;

            std::cout << "player: " << std::to_string(*(int*)event->peer->data) << " moved wrongly\n";
        }

        char* buffer = net_utills::convert_to_buffer(&back_p);

        net_utills::send_msg_safe(buffer, net_utills::get_packet_size(&back_p), event->peer, 0);

        delete[] buffer;


        //std::cout << "player \"" << id << "\" moved to " << std::to_string(pos.x) << ", " << std::to_string(pos.y) << "." << std::endl;
    } else {
        std::cerr << "[network] MOVE packet malformed or peer ID missing" << std::endl;
    }
}

void server_utls::send_player_list(ENetEvent *event, packet *p) {
    packet send_p;

    send_p.type = GET_PLAYER_LIST;

    std::vector<int> ids;

    ids.push_back(player_manager.get_host()->get_id());

    for (int i = 0; i < player_manager.players.size(); i++) {
        ids.push_back(player_manager.players[i]->get_id());
    }

    serialized_vector_int datad = net_utills::serializeIntVector(ids);

    send_p.size = datad.size;
    send_p.data = datad.buffer;

    char* buffer = net_utills::convert_to_buffer(&send_p);

    net_utills::send_msg_safe(buffer, net_utills::get_packet_size(&send_p), event->peer, 0);

    delete[] datad.buffer;

    delete[] buffer;
}

void server_utls::send_player_data(ENetEvent *event, packet *p) {
    packet send_p;

    send_p.type = GET_PLAYER;

    int* id = (int*)p->data;

    if (player_manager.fetch_player_data(*id) == nullptr) {
        std::cout << "id: " << std::to_string(*id) << "does not exist\n";
    }

    else {
        serialized_player srp = player_manager.fetch_player_data(*id)->serialize();

        send_p.size = sizeof(serialized_player);

        send_p.data = (char*)&srp;

        char* buffer = net_utills::convert_to_buffer(&send_p);

        net_utills::send_msg_fast(buffer, net_utills::get_packet_size(&send_p), event->peer, 0);

        delete[] buffer;
    }
}

void server_utls::handle_player_block_placement(ENetEvent *event, packet *p) {
    Vector2 b_p_pos;

    memcpy(&b_p_pos, p->data, sizeof(Vector2));

    char* name_b = new char[p->size - sizeof(Vector2)];

    memcpy(name_b, p->data + sizeof(Vector2), p->size - sizeof(Vector2));

    block blk;

    blk.state = 0;
    blk.attr = item_manager.fetch_item(name_b)->block_type_ptr;

    world.place_block(b_p_pos, blk);
}
