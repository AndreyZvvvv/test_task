#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "socket_client.h"

#define PORT 8888
#define MAX_READ_SIZE   10000000
char buffer[MAX_READ_SIZE] = {0};

using namespace std;

void debug_decode_buffer(char *buf){
    cout << "debug received array:" << endl;
    cout << "version: " << (int)buf[0] << endl;
    cout << "message_length: " << *((ssize_t*)&buf[1]) << endl;
    cout << "first num: " << *((double*)&buf[9 + 0*sizeof(double)]) << endl;
    cout << "50000: " << *((double*)&buf[9 + (50000-1)*sizeof(double)]) << endl;
    cout << "last: " << *((double*)&buf[9 + (1000000-1)*sizeof(double)]) << endl;
}

void SocketClient::interact_with_server(){
    sleep(3);
    double num = 56.78;
    send(sock , &num , sizeof(num) , 0 );
    printf("Number sent\n");

    char *buff_ptr = buffer;
    ssize_t bytes_read_total = 0;
    ssize_t bytes_read_at_the_iteration;
    ssize_t message_len = 0;
    int cnt = 0;
    while ((bytes_read_at_the_iteration = read( sock , buff_ptr, MAX_READ_SIZE)) > 0){
        //bytes 1-8 represents message length, it is read at first iteration
        if (cnt++ == 0){
            message_len = *(ssize_t*)&buff_ptr[1];
            cout << "message_len = " << message_len << endl;
        }
        buff_ptr += bytes_read_at_the_iteration;
        bytes_read_total += bytes_read_at_the_iteration;
        if (bytes_read_total >= message_len)
            break;
    }
    cout << "read count: " << cnt << endl;
    cout << "bytes_read_total: " << bytes_read_total << endl;

    debug_decode_buffer(buffer);

    close(sock);
}
   
int main(int argc, char const *argv[])
{
    SocketClient client = SocketClient("127.0.0.1", 8888);
    client.start();
    return 0;
}
