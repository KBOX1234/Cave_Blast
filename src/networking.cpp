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
        player* pla = new player;

        pla->set_id(random_num.get_random_int());
        pla->set_name("MASTER");
        pla->set_pos({0, 0});
        player_manager.players.push_back(pla);

        player_manager.host_id = pla->get_id();

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
            client_utls::player_creation_request("BINLADEN");
            //move_myself({10, 3});
        }
    }

}

network::~network() {
    enet_host_destroy(local_instance);
    enet_deinitialize();

}


void network::update() {
    if (is_server == true) {
        update_server();
    }

    else {
        update_client();
    }
}

bool network::is_host() {
    return is_server;
}
