#include <sstream>

using namespace std;

class File{
    private:
        stringstream file_buffer;
    public:
        File(const char* path);
        string get_value(const char* key_to_find);
};
