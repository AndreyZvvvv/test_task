#include <sys/types.h>
#include <netinet/in.h>

// struct to store a message and it's length
typedef struct{
    char* buff;
    size_t len;
}MessageToSend;

typedef MessageToSend (*response_handler)(char*);

class SocketServer{
    private:
        int master_socket;
        int addrlen;
        int client_socket[30]; 
        int max_clients;
        struct sockaddr_in address;
        //set of socket descriptors 
        fd_set readfds;
        //function to hadle and proccess income data
        response_handler user_handler;
        bool check_new_connection();
        void proccess_data();
        void eventloop();
    public:
        SocketServer(int max_clients_num, response_handler handler);
        void start();
};
