#include <arpa/inet.h>
#include <string>

using namespace std;

class SocketClient{
    private:
        int sock;
        struct sockaddr_in server_addr;
    public:
        SocketClient(string &ip, int port);
        void start();
        void interact_with_server();
};
