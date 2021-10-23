#include "socket_server.h"
#include <iostream>
#include <string.h>

using namespace std;

// void increment_num(double num, char *buff, size_t size){
//     for (int i=0; i<size; i++)
//     {
//         num++;
//         memcpy(&buff[i * sizeof(double)], &num, sizeof(double));
//     }
// }

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
    


    SocketServer server = SocketServer();
    server.start();

    return 0;
}