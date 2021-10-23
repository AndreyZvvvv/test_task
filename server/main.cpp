#include "socket_server.h"
#include <iostream>
#include <string.h>

#define DOUBLE_BUFF_SIZE   100000
char send_buffer[sizeof(double)*DOUBLE_BUFF_SIZE];

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
    MessageToSend message = {send_buffer, DOUBLE_BUFF_SIZE};
    double num;
    memcpy((void *)&num, received_data, sizeof(num));
    cout << "received num: " << num << endl;
    increment_num(num, send_buffer, DOUBLE_BUFF_SIZE);
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
    // char b[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4C, 0x40};

    // cout << "buffer: " << b << endl;
    // union double_in_bytes u;
    // memcpy(u.bytes, b, 8);
    // cout << "double: " << u.to_double << endl;
    


    SocketServer server = SocketServer(5, server_income_data_handler);
    server.start();

    return 0;
}