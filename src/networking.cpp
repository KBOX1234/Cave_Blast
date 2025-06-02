#include "../include/networking.hpp"

#include <iostream>

#include "../include/player.hpp"

void network::update() {
    if (is_server == true){
        server_obj.update();
    }

    else {
        client_obj.update();
    }
}

bool network::is_host() {
    return is_server;
}

void network::start(std::string ip, int port, bool is_server_s){
    if(is_server_s == true) {
        server_obj.start_server(ip, port);
    }
    else{
        client_obj.start_client(ip, port);
    }

    is_server = is_server_s;


}

ENetPeer* network::get_server(){
    return client_obj.remote_server;
}