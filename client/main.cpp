#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "socket_client.h"
#include "files.h"

using namespace std;

void debug_decode_buffer(char *buf){
    cout << "debug received array:" << endl;
    cout << "version: " << (int)buf[0] << endl;
    cout << "message_length: " << *((ssize_t*)&buf[1]) << endl;
    cout << "first num: " << *((double*)&buf[9 + 0*sizeof(double)]) << endl;
    cout << "50000: " << *((double*)&buf[9 + (50000-1)*sizeof(double)]) << endl;
    cout << "last: " << *((double*)&buf[9 + (1000000-1)*sizeof(double)]) << endl;
}
   
int main(int argc, char const *argv[])
{
    try{
        File config_file = File("client.conf");
        string addr = config_file.get_value("connection_address");
        string port_str = config_file.get_value("connection_port");
        string num_str = config_file.get_value("number");
        int port = stoi(port_str);
        double num = stod(num_str);

        SocketClient client = SocketClient(addr, port);
        client.start();
        sleep(3);
        client.send_message((char *)&num, sizeof(num));
        printf("Number sent\n");
        debug_decode_buffer(client.get_message());
    }
    catch(exception &e){
        cout << "Error. " << e.what() << endl;
    }

    return 0;
}
