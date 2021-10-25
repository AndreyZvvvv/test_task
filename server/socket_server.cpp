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
#define PROTOCOL_VERSION    (char)0x01

using namespace std;

SocketServer::SocketServer(int server_port, int max_clients_num, response_handler handler){
    int opt = TRUE;  
    max_clients = max_clients_num;
    user_handler = handler;
    port = server_port;
     
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
    address.sin_port = htons( port );  
         
    //bind the socket to localhost
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)  
    {  
        perror("bind failed");  
        exit(EXIT_FAILURE);  
    }  
    printf("Listener on port %d \n", port);  
         
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

void SocketServer::close_connection(int client_sock){
    getpeername(client_sock , (struct sockaddr*)&address , \
        (socklen_t*)&addrlen);  
    printf("Host disconnected , ip %s , port %d \n" , 
            inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
            
    //Close the socket and mark as 0 in list for reuse 
    close( client_sock );
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
            bytes_read_num = read( sd , receive_buffer, 1024);
            if (bytes_read_num == 0)
            {
                //Somebody disconnected , get his details and print 
                close_connection(sd);
                client_socket[i] = 0;  
            }         
            //send response
            else 
            {
                //proccessing message by the user application
                MessageToSend message = user_handler(receive_buffer);
                char *send_buffer = message.buff;
                send(sd, send_buffer, message.len, 0);
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
    }  
}
