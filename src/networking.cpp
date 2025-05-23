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

serialized_vector_int serializeIntVector(const std::vector<int>& vec) {
    serialized_vector_int result;
    result.size = vec.size() * sizeof(int);
    result.buffer = new char[result.size];
    std::memcpy(result.buffer, vec.data(), result.size);
    return result;
}

std::vector<int> deserializeIntVector(const char* buffer, size_t bufferSize) {
    size_t numInts = bufferSize / sizeof(int);
    std::vector<int> vec(numInts);
    std::memcpy(vec.data(), buffer, bufferSize);
    return vec;
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

void network::send_player_list_request() {
    packet* p = new packet;

    p->type = GET_PLAYER_LIST;
    p->size = 0;
    p->data = nullptr;

    char* buffer = net_utills::convert_to_buffer(p);

    send_msg_safe(buffer, net_utills::get_packet_size(p), remote_instance, 0);
}


void network::move_myself(Vector2 pos1) {
    packet* p = new packet;

    p->type = MOVE;
    p->size = sizeof(Vector2);

    p->data = new char[p->size];

    memcpy(p->data, &pos1, p->size);

    char* buff = net_utills::convert_to_buffer(p);

    if (buff == nullptr) {
        std::cout << "\nissue packing packet\n";
        return;
    }

    send_msg_safe(buff, net_utills::get_packet_size(p), remote_instance, 0);
    //delete[] static_cast<char*>(p->data);

    //delete p;
}

void network::player_creation_request(std::string name) {
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

    send_msg_safe(buff, net_utills::get_packet_size(p), remote_instance, 0);

    //delete[] static_cast<char*>(p->data);
    //delete p;
}

void network::fetch_player(int id) {
    packet* send_p = new packet;

    send_p->type = GET_PLAYER;
    send_p->size = sizeof(int);

    send_p->data = new char[sizeof(int)];

    memcpy(send_p->data, &id, sizeof(int));

    char* buffer = net_utills::convert_to_buffer(send_p);

    send_msg_safe(buffer, net_utills::get_packet_size(send_p), remote_instance, 0);
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

    if (player_manager.get_host() != nullptr) move_myself(player_manager.get_host()->get_pos());
    send_player_list_request();

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
                    std::cout << "my id is: " + std::to_string(player_manager.host_id) + "\n";
                    std::cout << "my official id is: " + std::to_string(player_manager.get_host()->get_id()) << std::endl;
                    std::cout << "my pos is: " + std::to_string(player_manager.get_host()->get_pos().x) + ", " + std::to_string(player_manager.get_host()->get_pos().x) << std::endl;
                    if (p->type == GET_PLAYER_LIST) {
                        std::vector<int> ids;

                        ids = deserializeIntVector(p->data, p->size);

                        for (int i = 0; i < ids.size(); i++) {
                            //std::cout << "id: " << std::to_string(ids[i]) << std::endl;

                            fetch_player(ids[i]);
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

                enet_packet_destroy(event.packet);
                break;
            } // End of scope for RECEIVE case

            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Server Disconnected\n";
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
