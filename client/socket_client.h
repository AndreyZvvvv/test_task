#include <arpa/inet.h>

class SocketClient{
    private:
        int sock;
        struct sockaddr_in server_addr;
    public:
        SocketClient(const char *ip, int port);
        void start();
        void interact_with_server();
};
