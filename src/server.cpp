#include "../include/networking.hpp"

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

    switch (p->type) {
        case CREATE_PLAYER:
            server_utls::handle_player_creation(event, p);
            break;
        case MOVE:
            server_utls::handle_move_packet(event, p);
            break;
        case GET_PLAYER_LIST:
            server_utls::send_player_list(event, p);
            break;
        case GET_PLAYER:
            server_utls::send_player_data(event, p);
            break;
        case SET_BLOCK:
            server_utls::handle_player_block_placement(event, p);
            break;
        case GET_ALL_PLAYERS:
            server_utls::handle_all_player_fetch(event);
            break;
        default:
            std::cerr << "Unknown packet type: " << static_cast<int>(p->type) << std::endl;
            break;
    }

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
