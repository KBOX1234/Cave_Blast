//isolated storage bank is used to give objects an arbitrary place to store constant data of any type

#pragma once

#include <vector>
#include <any>
#include <string>
#include <optional>

class isb {
    private:
        std::vector<std::any> data;
        std::vector<std::string> names;

        void remove_entry(int index);

        void add_entry(std::any object, const std::string& name);

        int find_object_index(const std::string& name);

    public:

        std::optional<std::any> get_object(const std::string& name);

        int free_object(const std::string& name);

};