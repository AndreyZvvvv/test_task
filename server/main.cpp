#include "socket_server.h"
#include <iostream>
#include <string.h>
#include "files.h"
/*
Packet description:
    [PROTOCOL_VERSION - 1byte][MESSAGE_LENGTH - 8bytes][DOUBLES - ...]
*/
#define PROTOCOL_VERSION    (unsigned char)0x01
#define DOUBLES_NUM         1000000
#define SEND_MESSAGE_LEN    1+sizeof(ssize_t)+DOUBLES_NUM*sizeof(double)    //1+8+...
char send_buffer[SEND_MESSAGE_LEN];

using namespace std;

void increment_num(double num, char *buff, size_t size){
    for (int i=0; i<size; i++)
    {
        num++;
        memcpy(&buff[i * sizeof(double)], &num, sizeof(double));
    }
}

MessageToSend interact_with_client(char *received_data){
    cout << "A message received from client" << endl;
    MessageToSend message = {send_buffer, SEND_MESSAGE_LEN};
    //convert received bytes to double
    double num;
    memcpy((void *)&num, received_data, sizeof(num));
    cout << "received num: " << num << endl;

    send_buffer[0] = PROTOCOL_VERSION;
    ssize_t message_len = (ssize_t)SEND_MESSAGE_LEN;
    memcpy(&send_buffer[1], &message_len, sizeof(ssize_t));
    increment_num(num, send_buffer+1+sizeof(ssize_t), DOUBLES_NUM);
    return message;
}

void debug_decode_buffer(char *buf, size_t size){
    cout << "debug received array:" << endl;
    for (int i=0; i<size; i++){
        double num;
        memcpy(&num, &buf[i*sizeof(double)], sizeof(double));
        cout << num << endl;
    }
}

int main(int argc , char *argv[])  
{
    File file = File("server.conf");
    string port_str = file.get_value("server_port");
    try{
        int port = stoi(port_str);
        cout << "server_port: " << port << endl;
        SocketServer server = SocketServer(port, 5, interact_with_client);
        server.start();
    }
    catch (exception &e){
        cout << "Error. " << e.what() << endl;
    }
    return 0;
}
