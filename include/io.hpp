class easy_file_ops{
    public:
        static std::string load_text_file(std::string filename);

        static void save_to_text_file(std::string data, std::string filename);

        static char* load_binary_file(std::string filename);

        static void save_binary_file(char* data, size_t size, std::string filename);
};
