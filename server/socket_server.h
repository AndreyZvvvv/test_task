#include <sys/types.h>
#include <netinet/in.h>

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
        char* (*response_handler)(char*);
        bool check_new_connection();
        void proccess_data();
        void eventloop();
    public:
        SocketServer(int max_clients_num, char* (*response_handler)(char*));
        void start();
};
