#include "socket_server.h"
#include <iostream>
#include <stdio.h> 
#include <string.h>   //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>   //close 
#include <arpa/inet.h>    //close 
#include <sys/socket.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
     
#define TRUE   1 
#define FALSE  0 
#define PORT 8888
#define PROTOCOL_VERSION    (char)0x01

using namespace std;

void increment_num(double num, char *buff, size_t size){
    for (int i=0; i<size; i++)
    {
        num++;
        memcpy(&buff[i * sizeof(double)], &num, sizeof(double));
    }
}

SocketServer::SocketServer(int max_clients_num, char* (*response_handler)(char*)){
    int opt = TRUE;  
    max_clients = 30; 
     
    //initialise all client_socket[] to 0 so not checked 
    for (int i = 0; i < max_clients; i++)  
    {  
        client_socket[i] = 0;  
    }  
         
    //create a master socket 
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)  
    {  
        perror("socket failed");  
        exit(EXIT_FAILURE);  
    }  
     
    //set master socket to allow multiple connections , 
    //this is just a good habit, it will work without this 
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
          sizeof(opt)) < 0 )  
    {  
        perror("setsockopt");  
        exit(EXIT_FAILURE);  
    }  
}

void SocketServer::start(){
    //type of socket created 
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;  
    address.sin_port = htons( PORT );  
         
    //bind the socket to localhost port 8888 
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)  
    {  
        perror("bind failed");  
        exit(EXIT_FAILURE);  
    }  
    printf("Listener on port %d \n", PORT);  
         
    //try to specify maximum of 3 pending connections for the master socket 
    if (listen(master_socket, 3) < 0)  
    {  
        perror("listen");  
        exit(EXIT_FAILURE);  
    }  
         
    //accept the incoming connection 
    addrlen = sizeof(address);  
    puts("Waiting for connections ...");

    eventloop();
}

bool SocketServer::check_new_connection(){
    //If something happened on the master socket , 
    //then its an incoming connection
    //else its some IO operation on some other socket
    int new_socket;

    if (FD_ISSET(master_socket, &readfds))  
    {  
        if ((new_socket = accept(master_socket, 
                (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)  
        {  
            perror("accept");  
            exit(EXIT_FAILURE);  
        }  
            
        //inform user of socket number - used in send and receive commands 
        printf("New connection , socket fd is %d , ip is : %s , port : %d"
                "\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs
                (address.sin_port)); 
                
        //add new socket to array of sockets 
        for (int i = 0; i < max_clients; i++)  
        {  
            //if position is empty 
            if( client_socket[i] == 0 )  
            {  
                client_socket[i] = new_socket;  
                printf("Adding to list of sockets as %d\n" , i);  
                        
                return true;  
            }  
        }  
    }
    return false;
}

void SocketServer::proccess_data(){    
    char receive_buffer[1025];  //data buffer of 1K 
    ssize_t bytes_read_num; 
    for (int i = 0; i < max_clients; i++)  
    {  
        int sd = client_socket[i];  
                
        if (FD_ISSET( sd , &readfds))  
        {  
            //Check if it was for closing , and also read the 
            //incoming message 
            if ((bytes_read_num = read( sd , receive_buffer, 1024)) == 0)  
            {  
                //Somebody disconnected , get his details and print 
                getpeername(sd , (struct sockaddr*)&address , \
                    (socklen_t*)&addrlen);  
                printf("Host disconnected , ip %s , port %d \n" , 
                        inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
                        
                //Close the socket and mark as 0 in list for reuse 
                close( sd );  
                client_socket[i] = 0;  
            }  
                    
            //send response
            else 
            {
                // receive_buffer[bytes_read_num] = '\0';  
                // send(sd , receive_buffer , strlen(receive_buffer) , 0 );
                double num;
                memcpy((void *)&num, receive_buffer, sizeof(num));
                cout << "received num: " << num << endl;
#define DOUBLE_BUFF_SIZE   100000
                char send_buffer[sizeof(double)*DOUBLE_BUFF_SIZE];
                increment_num(num, send_buffer, DOUBLE_BUFF_SIZE);
                send(sd, send_buffer, sizeof(double)*DOUBLE_BUFF_SIZE, 0);
            }  
        }  
    }  
}

void SocketServer::eventloop(){
    while(TRUE)  
    {  
        //clear the socket set 
        FD_ZERO(&readfds);  
     
        //add master socket to set 
        FD_SET(master_socket, &readfds);  
        int max_sd = master_socket;  
             
        //add child sockets to set 
        for (int i = 0 ; i < max_clients ; i++)  
        {  
            //socket descriptor 
            int sd = client_socket[i];  
                 
            //if valid socket descriptor then add to read list 
            if(sd > 0)  
                FD_SET( sd , &readfds);  
                 
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
        }  
     
        //wait for an activity on one of the sockets , timeout is NULL , 
        //so wait indefinitely 
        int activity = select(max_sd + 1 , &readfds , NULL , NULL , NULL);  
       
        if ((activity < 0) && (errno!=EINTR))  
        {  
            printf("select error");
        }  
              
        if (!check_new_connection()){
            proccess_data();
        }
        sleep(1);
    }  
}
