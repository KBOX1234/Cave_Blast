#include "networking.hpp"
#include "world.hpp"
#include "rng.hpp"
#include "httplib.h"
#include "player.hpp"
#include "lighting.hpp"

#include <iostream>

#include "npc.hpp"


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

                    if (p->type == GET_PLAYER_LIST) handle_player_list_response(&event, p);

                    if (p->type == GET_PLAYER) handle_player_get_response(&event, p);
                }

                if (p->type == CREATE_PLAYER) handle_player_creation_response(&event, p);

                if (p->type == SET_BLOCK) handle_block_updates(&event, p);

                if (p->type == DISCONNECT_PLAYER) handle_other_player_disconnect(&event, p);

                if (p->type == RE_CALIBRATE) handle_recalibrate_request_from_server(&event, p);

                if (p->type == GET_ALL_PLAYERS) handle_big_ahh_player_packet_with_all_players_from_the_server_for_which_data_is_comming_from(&event, p);

                if(p->type == GIVE_BLOCK) handel_get_item_from_server(&event, p);

                if (p->type == GET_NPC_LIST) handle_npc_updates(&event, p);


                enet_packet_destroy(event.packet);
                break;
            } // End of scope for RECEIVE case

            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "(CLIENT): Server Disconnected\n";
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
        std::cout << "(CLIENT): Connecting to server port: " << std::to_string(address.port) << std::endl;

        ENetEvent event;

        if (enet_host_service(myself, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
            std::cout << "(CLIENT): Connection established" << std::endl;
            //move_myself({10, 3});
        }

        else{
            std::cout << "(CLIENT): Could not connect\n";
            std::exit(EXIT_FAILURE);
        }
}

void client::handle_player_list_response(ENetEvent* event, packet* p){
    std::vector<int> ids;

    ids = net_utills::deserializeIntVector(p->data, p->size);

    for (int i = 0; i < ids.size(); i++) {
        //std::cout << "id: " << std::to_string(ids[i]) << std::endl;

        client_utls::fetch_player(ids[i], remote_server);
    }
}

void client::handle_player_get_response(ENetEvent* event, packet* p){
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

            light_manager.update_light_pos(pl->get_light_index(), spl.pos);

        }
        else {
            std::cerr << "player id: " + std::to_string(spl.id) + " does not exist\n";
        }
    }
}

void client::handle_player_creation_response(ENetEvent* event, packet* p){
    serialized_player splr;

    std::cout << "(CLIENT): New player\n(CLIENT): Player id: " << std::to_string(splr.id) << std::endl;

    memcpy(&splr, p->data, sizeof(serialized_player));

    player_manager.add_player_from_serialised_player(&splr);

    player_manager.host_id = splr.id;

    player_manager.myself->set_id(splr.id);
    //std::cout << "my id is: " << std::to_string(splr.id) << std::endl;

    if (player_manager.re_sync_timer < 2 * GetFPS()) {
        player_manager.re_sync_timer ++;
    }
    else {
        player_manager.myself->set_pos(player_manager.get_host()->get_pos());
        player_manager.myself->zero_rotation();
        player_manager.myself->increase_angle(player_manager.get_host()->get_rotation());
    }
}

void client::handle_block_updates(ENetEvent* event, packet* p){
    char* name = new char[p->size - sizeof(Vector2)];

    Vector2 pos;

    memcpy(&pos, p->data, sizeof(Vector2));

    memcpy(name, p->data + sizeof(Vector2), p->size - sizeof(Vector2));

    block blk;

    blk.state = 0;

    blk.attr = item_manager.fetch_item(name)->block_type_ptr;

    world.place_block(pos, blk);
}

void client::handle_other_player_disconnect(ENetEvent* event, packet* p){
    int id;

    memcpy(&id, p->data, sizeof(int));

    player_manager.remove_player(id);
}

void client::handle_recalibrate_request_from_server(ENetEvent* event, packet* p){
    Vector2 n_pos;

    memcpy(&n_pos, p->data, sizeof(Vector2));

    player_manager.get_host()->set_pos(n_pos);
}

void client::handle_big_ahh_player_packet_with_all_players_from_the_server_for_which_data_is_comming_from(ENetEvent* event, packet* p) {
    size_t size = (size_t)round(p->size / sizeof(serialized_player));
    size_t inc = 0;

    while (inc < p->size) {
        serialized_player spl;

        memcpy(&spl, p->data + inc, sizeof(serialized_player));
        inc += sizeof(serialized_player);

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

void client::handel_get_item_from_server(ENetEvent* client, packet* p){
    char count;

    memcpy(&count, p->data, sizeof(char));

    char tmp_str[p->size - sizeof(char)];

    memcpy(tmp_str, p->data + sizeof(char), p->size - sizeof(char));

    item* itm = item_manager.fetch_item(tmp_str);

    player_manager.myself->inv.give_item(itm, count);
}

void client::handle_npc_updates(ENetEvent *event, packet *p) {
    serialized_npc snpc;

    memcpy(&snpc, p->data, sizeof(serialized_npc));

    npc_manager.update_npc_from_serialized_npc(snpc);
}
