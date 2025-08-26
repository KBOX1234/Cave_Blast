#pragma once

#include "../external/raylib/src/raylib.h"
#include "../external/soloud/include/soloud.h"
#include "../external/soloud/include/soloud_wav.h"
#include <iostream>
#include <vector>

struct smart_sound {
    std::string name;

    SoLoud::Wav wave_data;

    int handle;
};

class sound_master {
    private:

        SoLoud::Soloud backend;

        std::vector<smart_sound*> sounds;

        smart_sound* find_sound_by_name(std::string name);

    public:

        sound_master();

        ~sound_master();

        //fname is the name of the file and sname is the name it will be indexed as
        void load_sound(std::string fname, std::string sname);

        void play_sound(std::string name, Vector2 pos, bool dynamic = false);
};

extern sound_master sound;

