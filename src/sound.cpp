#include "../include/sound.hpp"

smart_sound *sound_master::find_sound_by_name(std::string name) {
    for (int i = 0; i < sounds.size(); i++) {
        if (sounds[i]->name == name) {
            return sounds[i];
        }
    }

    return nullptr;
}

sound_master::sound_master() {
    backend.init();
    std::cout << "(SOUND_MANAGER): Initilised sound engine\n";
}

sound_master::~sound_master() {
    backend.deinit();

    for (int i = 0; i < sounds.size(); i++) {
        delete sounds[i];
    }
}


void sound_master::load_sound(std::string fname, std::string sname) {
    smart_sound* new_sound = new smart_sound;

    new_sound->name = sname;

    //keep this or our sounds will never work
    if (!new_sound->wave_data.load(fname.c_str())) {
        //std::cerr << "(SOUND_MANAGER) Failed to load sound: " << fname << std::endl;
    }

    sounds.push_back(new_sound);
}

void sound_master::play_sound(std::string name, Vector2 pos, bool dynamic) {
    smart_sound* sound = find_sound_by_name(name);

    if (sound == nullptr) {
        std::cout << "(SOUND_MANAGER): Failed to play sound (nullptr)\n";
        return;
    }

    if (dynamic == false) {
        SoLoud::handle h =
        sound->handle = backend.play(sound->wave_data);

        if (h == 0) std::cout << "(SOUND_MANAGER): Failed to play sound\n";
    }
    else {
        //just do the same for now
        sound->handle = backend.play3d(sound->wave_data, pos.x, pos.y, 1);
    }
}
