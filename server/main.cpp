#include "socket_server.h"
#include <iostream>
#include <string.h>
#include "files.h"
/*
Packets description:
    Client request:
        [PROTOCOL_VERSION - 1byte][MESSAGE_TYPE - 1byte - REQUEST_SEND_NUMBER][DOUBLE NUM]
    Server response success:
        [PROTOCOL_VERSION - 1byte][MESSAGE_TYPE - 1byte - RESPONSE_SUCCESS][MESSAGE_LENGTH - 8bytes][DOUBLES - ...]
    Server response error:
        [PROTOCOL_VERSION - 1byte][MESSAGE_TYPE - 1byte - RESPONSE_ERROR][MESSAGE_LENGTH - 8bytes];
*/
#define PROTOCOL_VERSION    (unsigned char)0x02
#define DOUBLES_NUM         1000000
#define SEND_MESSAGE_LEN    1+1+sizeof(ssize_t)+DOUBLES_NUM*sizeof(double)

#define REQUEST_SEND_NUMBER 0x01
#define RESPONSE_SUCCESS    0x02
#define RESPONSE_ERROR      0x03

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
    /*
    Received packet description:
    [PROTOCOL_VERSION - 1byte][MESSAGE_TYPE - 0x01(SEND NUMBER)][DOUBLE_NUM - 4bytes]
    */
    int protocol = (int)received_data[0];
    int message_type = (int)received_data[1];
    cout << "Client protocol version: " << protocol << endl <<
        "Message type: " << message_type << endl;
    if (protocol == PROTOCOL_VERSION && message_type == REQUEST_SEND_NUMBER){
        double num;
        memcpy((void *)&num, &received_data[2], sizeof(num));
        cout << "Received num: " << num << endl;

        send_buffer[0] = PROTOCOL_VERSION;
        send_buffer[1] = RESPONSE_SUCCESS;
        ssize_t message_len = (ssize_t)SEND_MESSAGE_LEN;
        memcpy(&send_buffer[2], &message_len, sizeof(ssize_t));
        increment_num(num, send_buffer+1+1+sizeof(ssize_t), DOUBLES_NUM);
    }
    else{
        cout << "Error. Conflicting protocol version" << endl;
        send_buffer[0] = PROTOCOL_VERSION;
        send_buffer[1] = RESPONSE_ERROR;
        ssize_t len = 2 + sizeof(ssize_t);
        memcpy(&send_buffer[2], &len, sizeof(ssize_t));
        message.len = 2;
    }
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
