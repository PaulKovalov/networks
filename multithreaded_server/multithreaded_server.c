#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

// use a structure to keep required data for chat() method
struct chat_args {
    int socket;
    struct sockaddr_in client_address;
};
int connections = 0;
int sockfd;
const int BUFFER_SIZE = 131072;
const int PORT = 8080;


void chat(void*);

int main()
{
    int newsockfd, client_length;

    struct sockaddr_in server_address, client_address;
    // create listening socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("error opening socket\n");
        return 0;
    }
    bzero((char *)&server_address, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        printf("error binding socket\n");
        return 0;
    }
    else
    {
        printf("success binding socket\n");
    }

    listen(sockfd, 5);
    client_length = sizeof(client_address);
    for (;;)
    {
        // when new connection arrives, create new socket specifically for that connection
        newsockfd = accept(sockfd, (struct sockaddr *)&client_address, &client_length);
        if (newsockfd < 0)
        {
            printf("error accepting\n");
        }
        // then move chat to the thread
        pthread_t thread_id;
        pthread_attr_t thread_attrs;
        pthread_attr_init(&thread_attrs);
        struct chat_args args;
        args.socket = newsockfd;
        args.client_address = client_address;
        pthread_create(&thread_id, &thread_attrs, chat, (void*)&args);
    }
    close(sockfd);
    return 0;
}

void chat(void* args)
{
    ++connections;
    printf("active connections %d\n", connections);
    struct chat_args *c_args = (struct chat_args*) args;
    int newsockfd = c_args->socket;
    struct sockaddr_in client_address = c_args->client_address;
    char buffer[BUFFER_SIZE];
    for (;;)
    {
        bzero(buffer, BUFFER_SIZE);
        if (recv(newsockfd, buffer, BUFFER_SIZE, 0) < 0)
        {
            printf("error reading from socket\n");
            break;
        }
        else
        {
            strtok(buffer, "\n");
            if (strcmp(buffer, "exit") == 0 || strlen(buffer) == 0)
            {
                printf("Received \"exit\" command from ip address %s, exiting\n", inet_ntoa(client_address.sin_addr));
                break;
            }
            printf("received message \"%s\" from ip address %s\n", buffer, inet_ntoa(client_address.sin_addr));
        }
        if (send(newsockfd, buffer, strlen(buffer), 0) < 0)
        {
            printf("error writing to socket\n");
        }
    }
    close(newsockfd);
    --connections;
    printf("active connections %d\n", connections);
    if (connections == 0) {
        printf("No active connections left, exiting\n");
        close(sockfd);
        exit(0);
    }
}