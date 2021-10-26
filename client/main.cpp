#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "socket_client.h"
#include "files.h"

using namespace std;

#define PROTOCOL_VERSION    (unsigned char)0x02
#define REQUEST_SEND_NUMBER 0x01
#define REQUEST_PACKET_SIZE 1+1+sizeof(double)
#define RESPONSE_SUCCESS    0x02
#define RESPONSE_ERROR      0x03

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
        char send_message[10] = {PROTOCOL_VERSION, REQUEST_SEND_NUMBER};
        memcpy(&send_message[2], &num, sizeof(num));
        client.send_message(send_message, REQUEST_PACKET_SIZE);
        cout << "Number sent" << endl;
        char * received_message = client.get_message();
        int protocol = (int)received_message[0];
        int message_type = (int)received_message[1];
        ssize_t message_len = *(ssize_t*)&received_message[2];
        cout << "A message received from server" << endl <<
            "protocol version: " << protocol << endl <<
            "message type: " << message_type << endl <<
            "message length: " << message_len << endl;
        if (message_type == RESPONSE_SUCCESS){
            File data_file("receive.dat");
            ssize_t offset = 1 + 1 + sizeof(ssize_t);
            data_file.write_binary_data(&received_message[offset], message_len-offset);
            cout << "Received data are written to receive.dat" << endl;
        }
        else{
            cout << "ERROR received from server. Conflicting protocol version." << endl;
        }
    }
    catch(exception &e){
        cout << "Error. " << e.what() << endl;
    }

    return 0;
}
