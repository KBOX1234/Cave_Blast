#include <iostream>

#define NOFILE "iul84y0ufp"

class easy_file_ops{
    public:
        static std::string load_text_file(std::string filename);

        static int save_to_text_file(std::string data, std::string filename);

        static char* load_binary_file(std::string filename);

        static void save_binary_file(char* data, size_t size, std::string filename);
};
