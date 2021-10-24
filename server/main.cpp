#include "socket_server.h"
#include <iostream>
#include <string.h>

#define DOUBLES_NUM   10
#define SEND_MESSAGE_LEN    DOUBLES_NUM*sizeof(double)+1
char send_buffer[SEND_MESSAGE_LEN];

using namespace std;

void increment_num(double num, char *buff, size_t size){
    for (int i=0; i<size; i++)
    {
        num++;
        memcpy(&buff[i * sizeof(double)], &num, sizeof(double));
    }
}

MessageToSend server_income_data_handler(char *received_data){
    cout << "A message received from client" << endl;
    MessageToSend message = {send_buffer, SEND_MESSAGE_LEN};
    //convert received bytes to double
    double num;
    memcpy((void *)&num, received_data, sizeof(num));
    cout << "received num: " << num << endl;
    increment_num(num, send_buffer, DOUBLES_NUM);
    //termination symbol
    send_buffer[SEND_MESSAGE_LEN - 1] = '!';
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
    SocketServer server = SocketServer(5, server_income_data_handler);
    server.start();

    return 0;
}
