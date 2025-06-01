#include "../include/networking.hpp"

#include <iostream>

#include "../include/player.hpp"

std::unique_ptr<client> networking_client = nullptr;

void client::update() {

    if (player_manager.get_host() != nullptr) client_utls::move_myself(player_manager.get_host()->get_rotation(), player_manager.get_host()->get_pos(), remote_server);
    //client_utls::fetch_all_players();
    client_utls::send_player_list_request(remote_server);

    ENetEvent event;
    /* Wait up to 0 milliseconds for an event. */
    while (enet_host_service(myself, &event, 0) > 0)
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

                        ids = net_utills::deserializeIntVector(p->data, p->size);

                        for (int i = 0; i < ids.size(); i++) {
                            //std::cout << "id: " << std::to_string(ids[i]) << std::endl;

                            client_utls::fetch_player(ids[i], remote_server);
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

//new stuff

void client::start_client(std::string ip, int port){
        ip_addr = ip;

        std::string url = "http://localhost:" + std::to_string(port);


        try {
            cli = std::make_unique<httplib::Client>(ip_addr, port);

        } catch (const std::exception& e) {
            std::cerr << "Failed to initialize httplib::Client: " << e.what() << std::endl;
        }



        myself = enet_host_create(NULL /* create a client host */,
            1 /* only allow 1 outgoing connection */,
            2 /* allow up 2 channels to be used, 0 and 1 */,
            0 /* assume any amount of incoming bandwidth */,
            0 /* assume any amount of outgoing bandwidth */);

        if (myself == NULL) {
            std::cerr << "Error creating client socket" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        ENetAddress address = {};

        enet_address_set_host(&address, ip_addr.c_str());
        address.port = port;

        remote_server = enet_host_connect(myself, &address, 2, 0);

        if (remote_server == NULL) {
            std::cerr << "Error connecting to server" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        std::cout << "connecting to server port: " << std::to_string(address.port) << std::endl;

        ENetEvent event;

        if (enet_host_service(myself, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
            std::cout << "Connection established" << std::endl;
            //move_myself({10, 3});
        }

        else{
            std::cout << "could not connect\n";
            std::exit(EXIT_FAILURE);
        }
}

void client::fetch_chunk(Vector2 pos) {
    async_chunk_fetch_on = true;
    json sendJ;

    sendJ["x"] = pos.x;
    sendJ["y"] = pos.y;

    auto res = cli->Post("/chunk", sendJ.dump(), "application/json");

    if (res && res->status == 200) {

        //std::cout << "Response:\n" << res->body << std::endl;
        json new_c = json::parse(res->body);
        world.new_chunk_from_json(new_c);

    }
    async_chunk_fetch_on = false;
}