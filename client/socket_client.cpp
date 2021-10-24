#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 8888
#define MAX_READ_SIZE   1000000
char buffer[MAX_READ_SIZE] = {0};

using namespace std;

void debug_decode_buffer(char *buf, size_t size){
    cout << "debug received array:" << endl;
    for (int i=0; i<size; i++){
        double num;
        memcpy(&num, &buf[i*sizeof(double)], sizeof(double));
        cout << num << endl;
    }
}

class SocketClient{
    private:
        int sock;
        struct sockaddr_in server_addr;
    public:
        SocketClient(char *ip, int port);
        void start();
        void interact_with_server();
};

SocketClient::SocketClient(char *ip, int port){
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return;
    }
   
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
       
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, ip, &server_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return;
    }
}

void SocketClient::start(){
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("\nConnection Failed \n");
    }
    else{
        interact_with_server();
    }
    
}

void SocketClient::interact_with_server(){
    sleep(3);
    double num = 56.78;
    send(sock , &num , sizeof(num) , 0 );
    printf("Number sent\n");

    char *buff_ptr = buffer;
    ssize_t bytes_read_num;
    ssize_t message_len = 0;
    while ((bytes_read_num = read( sock , buff_ptr, MAX_READ_SIZE)) > 0){
        buff_ptr += bytes_read_num;
        message_len += bytes_read_num;
        if (*(buff_ptr - 1) == '!'){
            break;
        }
    }
    cout << "message_len = " << message_len << endl;
    close(sock);
}
   
int main(int argc, char const *argv[])
{
    SocketClient client = SocketClient("127.0.0.1", 8888);
    client.start();
    return 0;
}
