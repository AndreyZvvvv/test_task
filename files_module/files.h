#include <sstream>
#include <string>

using namespace std;

class File{
    private:
        string file_path;
        stringstream file_buffer;
    public:
        File(const char* path);
        string get_value(const char* key_to_find);
        void write_binary_data(char *buf, int size);
};
