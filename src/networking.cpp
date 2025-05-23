#include "../include/networking.hpp"

#include <iostream>

#include "../include/player.hpp"

network::network() {
    std::string input;
    std::cout << "Server or Client (1, 2): ";
    std::cin >> input;

    if (input == "1") {
        is_server = true;
    }
    else is_server = false;

    if (enet_initialize() != 0) {
        std::cerr << "Failed to initialize ENet." << std::endl;
        std::exit(EXIT_FAILURE);
    }



    if (is_server) {
        address.host = ENET_HOST_ANY;
        address.port = PORT;

        local_instance = enet_host_create(&address /* the address to bind the server host to */,
        32			/* allow up to 32 clients and/or outgoing connections */,
        2	/* allow up to 2 channels to be used, 0 and 1 */,
        0			/* assume any amount of incoming bandwidth */,
        0			/* assume any amount of outgoing bandwidth */);

        if (local_instance == NULL) {
            std::cerr << "Error creating server socket" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        std::cout << "Server listening on port: " << std::to_string(address.port) << std::endl;


    }
    else {
        local_instance = enet_host_create(NULL /* create a client host */,
            1 /* only allow 1 outgoing connection */,
            2 /* allow up 2 channels to be used, 0 and 1 */,
            0 /* assume any amount of incoming bandwidth */,
            0 /* assume any amount of outgoing bandwidth */);

        if (local_instance == NULL) {
            std::cerr << "Error creating client socket" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        ENetAddress address = {};
        //enet_address_set_host(&address, "127.0.0.1");
        enet_address_set_host(&address, "localhost");
        address.port = PORT;

        remote_instance = enet_host_connect(local_instance, &address, 2, 0);

        if (remote_instance == NULL) {
            std::cerr << "Error connecting to server" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        std::cout << "connected to server port: " << std::to_string(address.port) << std::endl;

        ENetEvent event;

        if (enet_host_service(local_instance, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
            std::cout << "Connection established" << std::endl;
            player_creation_request("BINLADEN");
            move_myself({10, 3});
        }
    }

}

network::~network() {
    enet_host_destroy(local_instance);
    enet_deinitialize();

}

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
        std::cout  << "Player created" << std::endl;
        // Assume null-terminated string for name
        std::string new_name(p->data, p->size);  // safer than p->size - 1 unless you guarantee null-term

        int p_id = player_manager.add_player(new_name);

        int* p_id_ptr = new int(p_id);
        event->peer->data = p_id_ptr;  // Store player ID on peer

        std::cout << "player id: " << std::to_string(p_id) << std::endl;
    }

    else if (p->type == MOVE) {
        int* id_ptr = static_cast<int*>(event->peer->data);
        if (id_ptr && p->size >= sizeof(Vector2)) {
            int id = *id_ptr;
            Vector2 pos;
            std::memcpy(&pos, p->data, sizeof(Vector2));
            player_manager.players[id]->set_pos(pos);

            std::cout << "player \"" << id << "\" moved to " << std::to_string(pos.x) << ", " << std::to_string(pos.y) << "." << std::endl;
        } else {
            std::cerr << "[network] MOVE packet malformed or peer ID missing" << std::endl;
        }
    }

    delete[] p->data;
    delete p;
}



void network::handle_disconnect(ENetEvent *event) {
    std::cout <<  "A client disconnected" << std::endl;
}

void network::move_myself(Vector2 pos1) {
    packet* p = new packet;

    p->type = MOVE;
    p->size = sizeof(Vector2);

    p->data = new char[p->size];

    memcpy(p->data, &pos1, p->size);

    send_msg_safe((char*)p, net_utills::get_packet_size(p), remote_instance, 0);
    //delete[] static_cast<char*>(p->data);

    //delete p;
}

void network::player_creation_request(std::string name) {
    packet* p = new packet;

    p->type = CREATE_PLAYER;
    p->size = name.size() + 1;  // include null terminator

    p->data = new char[p->size];
    memcpy(p->data, name.c_str(), p->size);

    send_msg_safe((char*)p, net_utills::get_packet_size(p), remote_instance, 0);

    //delete[] static_cast<char*>(p->data);
    //delete p;
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

void network::update_client() {

    ENetEvent event;
    /* Wait up to 0 milliseconds for an event. */
    while (enet_host_service(local_instance, &event, 0) > 0)
    {
        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                printf("A packet of length %u containing %s was received on channel %u.\n",
                    event.packet->dataLength,
                    event.packet->data,
                    event.channelID);
                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy(event.packet);

                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Server Disconected\n";
                break;
        }
    }
}


void network::update() {
    if (is_server == true) {
        update_server();
    }

    else {
        update_client();
    }
}
