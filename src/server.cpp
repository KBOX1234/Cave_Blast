#include "networking.hpp"
#include "world.hpp"
#include "rng.hpp"
#include "httplib.h"
#include "player.hpp"

void server::handle_connect(ENetEvent *event) {
    printf("A new client connected from %x:%u.\n",
    event->peer->address.host,
    event->peer->address.port);

    clients.push_back(event->peer);
}

void server::handle_request(ENetEvent* event) {
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
        case BREAK_BLOCK:

            server_utls::handle_player_break_block(event, p);

            break;
        default:
            std::cerr << "Unknown packet type: " << static_cast<int>(p->type) << std::endl;
            break;
    }

    delete[] p->data;
    delete p;
}


void server::handle_disconnect(ENetEvent *event) {
    int id;

    memcpy(&id, event->peer->data, sizeof(int));

    std::cout <<  "client disconnected: "<< std::to_string(id) << std::endl;

    player_manager.remove_player(id);
    broadcast_disconnect(event);
}

void server::update() {
    ENetEvent event = {};

    while (enet_host_service(host_server, &event, 0) > 0)
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

    broadcast_block_changes();
}

void server::start_server(const std::string ip, int port2){

    ip_addr = ip;
    port = port2;


    if (enet_initialize() != 0) {
        std::cerr << "Failed to initialize ENet." << std::endl;
        std::exit(EXIT_FAILURE);
    }


        //prepare server owner so things can happen
        player* pla = new player;

        pla->set_id(random_num.get_random_int());
        pla->set_name("server");
        pla->set_pos({0, 0});
        player_manager.players.push_back(pla);

        player_manager.host_id = pla->get_id();

        player_manager.myself->set_id(pla->get_id());


        //do stuff so we can connect
        address.host = ENET_HOST_ANY;
        address.port = port;

        host_server = enet_host_create(&address /* the address to bind the server host to */,
        32			/* allow up to 32 clients and/or outgoing connections */,
        2	/* allow up to 2 channels to be used, 0 and 1 */,
        0			/* assume any amount of incoming bandwidth */,
        0			/* assume any amount of outgoing bandwidth */);

        if (host_server == NULL) {
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


            res.set_content(response.dump(), "application/json");


        });

        std::cout << "API server at http://localhost:" << std::to_string(port) << std::endl;
        std::thread server_thread(&server::start_api, this);
        server_thread.detach();
}

void server::start_api() {
    svr.listen(ip_addr, port);
}

server::~server() {
    enet_host_destroy(host_server);
    enet_deinitialize();
    svr.stop();

}

void server::broadcast_block_changes(){
    for (size_t i = 0; i < clients.size(); ++i) {
        ENetPeer* peer = clients[i];

        send_block_changes(peer);
        std::cout << "block changes size: " << std::to_string(blk_change.size() * sizeof(block_change)) << std::endl;
        std::cout << "number of changes: " << std::to_string(blk_change.size()) << std::endl;
        std::cout << "size of block_change: " << std::to_string(sizeof(block_change)) << std::endl;
    }
    blk_change.clear();
    
}

void server::broadcast_disconnect(ENetEvent* event){
    packet pp;

    pp.type = DISCONNECT_PLAYER;

    pp.size = sizeof(int);

    pp.data = (char*)event->peer->data;

    char* buffer = net_utills::convert_to_buffer(&pp);

    for (int i = 0; i < clients.size(); i++) {
        net_utills::send_msg_safe(buffer, net_utills::get_packet_size(&pp), clients[i], 0);

        int in;
        memcpy(&in, clients[i]->data, sizeof(int));

        std::cout << "player ids to send: " << std::to_string(in) << std::endl;
    }
}

ENetPeer* server::get_peer_by_player_id(int id){
    for (int i = 0; i < clients.size(); i++) {
        int id2;

        memcpy(&id2, clients[i]->data, sizeof(int));

        if(id2 == id){
            return clients[i];
        }
    }

    //this threat motivated this function to finaly work properly
    std::cout << "kill yourself\n";
    int* pp = NULL;
    pp[99] = 0;

    return nullptr;
}