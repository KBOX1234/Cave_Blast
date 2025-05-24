#include "../include/networking.hpp"

#include <iostream>

#include "../include/player.hpp"

void network::handle_connect(ENetEvent *event) {
    printf("A new client connected from %x:%u.\n",
    event->peer->address.host,
    event->peer->address.port);
}


void network::handle_request(ENetEvent* event) {
    ENetPacket* epacket = event->packet;
    char* buffer = reinterpret_cast<char*>(epacket->data);
    size_t buffer_size = epacket->dataLength;

    packet* p = net_utills::convert_from_buffer(buffer, buffer_size);
    if (!p) {
        std::cerr << "[network] Failed to deserialize packet" << std::endl;
        return;
    }

    if (p->type == CREATE_PLAYER) {
        std::string new_name(p->data, p->size);
        int p_id = player_manager.add_player(new_name);

        auto* p_id_ptr = new int(p_id);
        event->peer->data = p_id_ptr;

        player* plr = player_manager.fetch_player_data(p_id);

        serialized_player splr = plr->serialize();

        packet* send_p = new packet;
        send_p->type = CREATE_PLAYER;
        send_p->size = sizeof(serialized_player);
        send_p->data = (char*)&splr;

        char* buffer = net_utills::convert_to_buffer(send_p);
        send_msg_safe(buffer, net_utills::get_packet_size(send_p), event->peer, 0);

        //delete[] send_p->data;
        delete send_p;
    }

    else if (p->type == MOVE) {
        int* id_ptr = static_cast<int*>(event->peer->data);
        if (id_ptr && p->size >= sizeof(Vector2)) {
            int id = *id_ptr;
            Vector2 pos;
            std::memcpy(&pos, p->data, sizeof(Vector2));
            player_manager.players[id]->set_pos(pos);

            //std::cout << "player \"" << id << "\" moved to " << std::to_string(pos.x) << ", " << std::to_string(pos.y) << "." << std::endl;
        } else {
            std::cerr << "[network] MOVE packet malformed or peer ID missing" << std::endl;
        }
    }

    else if (p->type == GET_PLAYER_LIST) {
        packet* send_p = new packet;

        send_p->type = GET_PLAYER_LIST;

        std::vector<int> ids;

        ids.push_back(player_manager.get_host()->get_id());

        for (int i = 0; i < player_manager.players.size(); i++) {
            ids.push_back(player_manager.players[i]->get_id());
        }

        serialized_vector_int datad = serializeIntVector(ids);

        send_p->size = datad.size;
        send_p->data = datad.buffer;

        char* buffer = net_utills::convert_to_buffer(send_p);

        send_msg_safe(buffer, net_utills::get_packet_size(send_p), event->peer, 0);

    }

    else if (p->type == GET_PLAYER) {
        packet* send_p = new packet;

        send_p->type = GET_PLAYER;

        int* id = (int*)p->data;

        if (player_manager.fetch_player_data(*id) == nullptr) {
            std::cout << "id: " << std::to_string(*id) << "does not exist\n";
        }

        else {
            serialized_player srp = player_manager.fetch_player_data(*id)->serialize();

            send_p->size = sizeof(serialized_player);

            send_p->data = (char*)&srp;

            char* buffer = net_utills::convert_to_buffer(send_p);

            send_msg_safe(buffer, net_utills::get_packet_size(send_p), event->peer, 0);
        }

    }

    delete[] p->data;
    delete p;
}



void network::handle_disconnect(ENetEvent *event) {
    std::cout <<  "A client disconnected" << std::endl;
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
                /*printf("A packet of length %u containing %s was received on channel %u.\n",
                    event.packet->dataLength,
                    event.packet->data,
                    event.channelID);*/
                handle_request(&event);
                enet_packet_destroy(event.packet);


                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT:
            {
                printf("client disconnected.\n");

                break;
            }

        }


    }
}