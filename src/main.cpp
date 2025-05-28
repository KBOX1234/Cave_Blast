#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <cmath>

#include "raylib.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOGDI          // Avoid GDI definitions like Rectangle()
#define NOMINMAX       // Avoid min/max macros
#define NOSERVICE
#define NOHELP
#define NOATOM
#define NOCOMM
#define NOKANJI
#define NOWH
#define NOUSER
#define NOMCX
#define NOTAPE
#endif

#include <enet/enet.h>

/*#ifdef _WIN32
#define PlaySound WindowsPlaySound
#undef PlaySound

#endif*/


#include "imgui.h"
#include "rlImGui.h"
#include "json.hpp"

using json = nlohmann::json;
#include "rng.hpp"
rng_device random_num;
#include "texture_master.hpp"
texture_master texture_manager;
#include "item_master.hpp"
item_master item_manager;
#include "block_master.hpp"
block_master block_manager;
#include "world.hpp"
world_class world;
#include "player.hpp"
player_master player_manager;
#include "render.hpp"
render render_master;
#include "imgui_window.hpp"
imgui_win imgui_master;
#include "input.hpp"
keybind_master keybind_manager;
input input_manager;
#include "networking.hpp"
#include "delta_time.hpp"
delta_time delta_time_master;


#include "rng.hpp"
#include "world.hpp"

int main(int argc, char* argv[]) {

    int port = 0;
    bool server = false;
    std::string ip = "";
    std::string name = "";

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            int val = std::stoi(argv[++i]);
            server = (val != 0);
        }
        else if (strcmp(argv[i], "-ip") == 0 && i + 1 < argc) {
            ip = argv[++i];
        }
        else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            name = argv[++i];
        }
        else {
            std::cerr << "Unknown or incomplete argument: " << argv[i] << std::endl;
            return 1;
        }
    }

    std::cout << "Port: " << port << "\nServer: " << server << "\nIP: " << ip << std::endl;



    networking = std::make_unique<network>(server, ip, port);

    if (server == false) client_utls::player_creation_request(name);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGTH, "Cave Blast");
    rlImGuiSetup(true);

    texture_manager.set_default_texture("reasource/gfx/default.png");
    item_manager.load_item_declaration_file("reasource/items.json");

    player_manager.init();

    //SetTargetFPS(60);


    while (!WindowShouldClose()) {

        delta_time_master.update();
        networking->update();

        //everything that needs delta time:

        if (delta_time_master.can_game_continue() == true) {
            texture_manager.update();

            if (player_manager.get_host() != nullptr) {
                player_manager.update_predicted_player();
            }
        }

        if (player_manager.get_host() != nullptr) {
            input_manager.update();
        }


        BeginDrawing();
        ClearBackground(BLACK);

        if (player_manager.get_host() != nullptr) {

            render_master.update();

        }


        rlImGuiBegin();
        if (player_manager.get_host() != nullptr) {
            imgui_master.items_menu();

            imgui_master.player_data_menu();
        }
        rlImGuiEnd();

        DrawFPS(10, 10);



        EndDrawing();
    }
    texture_manager.clean_up();
    CloseWindow();
    rlImGuiShutdown();
    return 0;
}
