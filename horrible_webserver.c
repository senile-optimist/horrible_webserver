#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Maximum application buffer
#define APP_MAX_BUFFER 1024
#define PORT 8080
int main(){
    
    // Define the server and client file descriptors
    int server_fd, client_fd;

    // Define the socket address
    struct sockaddr_in address;
    int address_len = sizeof(address);
    // printf("%d\n",address_len);

    // Define the application buffer where we receive the requests
    // data will be moved from the receive buffer to here
    char buffer[APP_MAX_BUFFER]={0};

    // Create socket
    if((server_fd= socket(AF_INET, SOCK_STREAM,0))==0){
        perror("Socket Failed");
        exit(EXIT_FAILURE);
    }

    // binding  
    address.sin_family  = AF_INET; //ipv4
    address.sin_addr.s_addr = INADDR_ANY; //listen 0.0.0.0
    address.sin_port = htons(PORT);


    if(bind(server_fd,(struct sockaddr *)&address,sizeof(address))<0){
        perror("Bind Failed");
        exit(EXIT_FAILURE);
    }


    // Creating the queues
    // Listen for clients with 10 backlog(10 connections in accept queue)
    if(listen(server_fd,10)<0){
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    // We loop forever
    while(1){
        printf("\nWaiting for a connection...\n");

        // Accept a client connection client_fd == connection
        // If the accept queue is empty then we are stuck here
        if((client_fd = accept(server_fd,(struct sockaddr *)&address, (socklen_t*)&address_len))<0){
            perror("Accept failed");
            exit(EXIT_FAILURE); 
        }


        // read data from the OS receive buffer to the application (buffer)
        // this is essentialy reading the HTTP request
        // Dont bite more than you can chew APP_MAX_BUFFER
        read(client_fd,buffer,APP_MAX_BUFFER);
        printf("%s\n", buffer);


        // We send the request by writing to the socket send buffer in the os
        char *http_response = "HTTP/1.1 200 OK\n"
                            "Content-Type: text/plain\n"
                            "Content-Length: 13\n\n"
                            "Hello world!\n";

        // write to the socket
        // send queue
        write(client_fd, http_response, strlen(http_response)); 
        
        // close the client socket (terminate the tcp connection)
        close(client_fd);

    }


    return 0;

}