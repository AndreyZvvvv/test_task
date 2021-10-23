#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 8888
#define MAX_READ_SIZE   1000000

using namespace std;

void debug_decode_buffer(char *buf, size_t size){
    cout << "debug received array:" << endl;
    for (int i=0; i<size; i++){
        double num;
        memcpy(&num, &buf[i*sizeof(double)], sizeof(double));
        cout << num << endl;
    }
}
   
int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[MAX_READ_SIZE] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
       
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    // sleep(3);

    // char *hello = "Hello from client";
    // send(sock , hello , strlen(hello) , 0 );
    // printf("Hello message sent\n");
    // valread = read( sock , buffer, 1024);
    // printf("%s\n",buffer );
    // close(sock);
    // return 0;

    sleep(3);
    double num = 56.78;
    send(sock , &num , sizeof(num) , 0 );
    printf("Number sent\n");
    // sleep(5);
    valread = read( sock , buffer, MAX_READ_SIZE);
    cout << "valread = " << valread << endl;
    // debug_decode_buffer(buffer, 10);
    close(sock);
    return 0;
}
