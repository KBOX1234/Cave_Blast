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
    }

}

network::~network() {
    enet_host_destroy(local_instance);
}

void network::handle_connect(ENetEvent *event) {
    printf("A new client connected from %x:%u.\n",
    event->peer->address.host,
    event->peer->address.port);
}

void network::handle_request(ENetEvent *event) {
    packet* p = reinterpret_cast<packet*>(event->packet->data);

    if (p->type == CREATE_PLAYER) {
        char* name =  reinterpret_cast<char*>(p->data);

        name[p->size - 1] = '\0';

        std::string new_name(name);

        int p_id = player_manager.add_player(name);

        int* p_id_ptr = new int;

        memcpy(p_id_ptr, &p_id, sizeof(int));

        event->peer->data = p_id_ptr;


    }

    if (p->type == MOVE) {
        int* id_ptr = static_cast<int*>(event->peer->data);
        if (id_ptr) {
            int id = *id_ptr;

            Vector2 pos = *reinterpret_cast<Vector2*>(p->data);
            player_manager.players[id]->set_pos(pos);
        }


    }



    /* Clean up the packet now that we're done using it. */

}

void network::handle_disconnect(ENetEvent *event) {

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


void network::update() {
    if (is_server == true) {
        update_server();

    }
}
