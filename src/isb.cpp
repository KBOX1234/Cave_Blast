#include "../include/isb.hpp"

void isb::remove_entry(int index) {
    data.erase(data.begin() + index);
    names.erase(names.begin() + index);
}

void isb::add_entry(std::any object, const std::string& name) {
    data.push_back(object);
    names.push_back(name);
}

int isb::find_object_index(const std::string& name) {
    for (int i = 0; i < names.size(); i++) {
        if (names[i] == name) {
            return i;
        }
    }

    return -1;
}

int isb::free_object(const std::string& name) {
    int index = find_object_index(name);

    if (index == -1) return -1;

    remove_entry(index);
    return 0;
}

std::optional<std::any> isb::get_object(const std::string& name) {
    int index = find_object_index(name);

    if (index < 0 || index >= data.size()) {
        return std::nullopt; // object not found
    }

    return data[index];
}

bool isb::does_object_exist(const std::string &name) {
    int does = find_object_index(name);

    if (does != -1) {
        return true;
    }
    return false;
}




