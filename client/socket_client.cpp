#include "socket_client.h"
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

using namespace std;

#define MAX_READ_SIZE   10000000
char buffer[MAX_READ_SIZE] = {0};

void* read_thread_func(void* arg){
    SocketClient *client = (SocketClient *) arg;
    cout << "SocketClient Read thread has started" << endl;
    while(true){
        client->read_in_chunks();
    }
    pthread_exit(NULL);
}

SocketClient::SocketClient(string &ip, int port){
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return;
    }
   
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
       
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return;
    }
}

void SocketClient::read_in_chunks(){
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
}

void SocketClient::start(){
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("\nConnection Failed \n");
    }
    else{
        // interact_with_server();
        pthread_t read_thread;
        pthread_create(&read_thread, NULL, read_thread_func, this);
    }
}