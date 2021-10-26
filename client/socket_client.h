#include <arpa/inet.h>
#include <string>
#include <semaphore.h>

using namespace std;

class SocketClient{
    private:
        int sock;
        struct sockaddr_in server_addr;
    public:
        SocketClient(string &ip, int port);
        ~SocketClient();
        void start();
        void read_in_chunks();
        void interact_with_server();
        char* get_message();
        void send_message(char *buf, int size);
};
