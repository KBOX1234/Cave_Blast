#include "../include/networking.hpp"

#include <iostream>

#include "../include/player.hpp"

std::unique_ptr<network> networking = nullptr;

void network::start_api() {
    svr.listen(ip_addr, port);
}

network::network(bool server, const std::string ip, int port) : is_server(server), ip_addr(ip), port(port){


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

        player_manager.myself->set_id(pla->get_id());

        address.host = ENET_HOST_ANY;
        address.port = port;

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

        svr.Post("/chunk", [](const httplib::Request& req, httplib::Response& res) {

            //std::cout << "got request:\n" + req.body;

            json body = json::parse(req.body);

            Vector2 pos;



            pos.x = body["x"];

            pos.y = body["y"];



            chunk* chnk = world.get_chunk(pos);

            if (chnk == nullptr) {
                chnk = world.generate_chunk(pos);
            }

            json response = chnk->serialize_chunk();

            //std::cout << response.dump(4);
            //std::cout << "got request for chunk: " << std::to_string(pos.x) + ", " + std::to_string(pos.y) << std::endl;


            res.set_content(response.dump(4), "application/json");


        });

        std::cout << "API server at http://localhost:" << std::to_string(port) << std::endl;
        std::thread server_thread(&network::start_api, this);
        server_thread.detach();


    }
    else {
        //httplib::Client cli("http://localhost:" + std::to_string(API_PORT));
        std::string url = "http://localhost:" + std::to_string(port);


        try {
            cli = std::make_unique<httplib::Client>(ip_addr, port);

        } catch (const std::exception& e) {
            std::cerr << "Failed to initialize httplib::Client: " << e.what() << std::endl;
        }



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
        enet_address_set_host(&address, ip_addr.c_str());
        address.port = port;

        remote_instance = enet_host_connect(local_instance, &address, 2, 0);

        if (remote_instance == NULL) {
            std::cerr << "Error connecting to server" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        std::cout << "connected to server port: " << std::to_string(address.port) << std::endl;

        ENetEvent event;

        if (enet_host_service(local_instance, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
            std::cout << "Connection established" << std::endl;
            //move_myself({10, 3});
        }
    }





}

network::~network() {
    enet_host_destroy(local_instance);
    enet_deinitialize();
    svr.stop();

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

void network::fetch_chunk(Vector2 pos) {
    networking->async_chunk_fetch_on = true;
    json sendJ;

    sendJ["x"] = pos.x;
    sendJ["y"] = pos.y;

    auto res = networking->cli->Post("/chunk", sendJ.dump(), "application/json");

    if (res && res->status == 200) {

        //std::cout << "Response:\n" << res->body << std::endl;
        json new_c = json::parse(res->body);
        world.new_chunk_from_json(new_c);

    }
    networking->async_chunk_fetch_on = false;
}
