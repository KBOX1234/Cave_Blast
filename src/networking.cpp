#include "../include/networking.hpp"

#include <iostream>

network::network() {
    std::string input;
    std::cout << "Server or Client (1, 2): ";
    std::cin >> input;

    if (input == "1") {
        bool is_server = true;
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
