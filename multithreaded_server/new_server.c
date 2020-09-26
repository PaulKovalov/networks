#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

const int BUFFER_SIZE = 131072;
const int PORT = 8080;

void chat(int, int, struct sockaddr_in);

int main()
{
    int sockfd, newsockfd, client_length;
    
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
    // when new connection arrives, create new socket specifically for that connection
    newsockfd = accept(sockfd, (struct sockaddr *)&client_address, &client_length);
    if (newsockfd < 0)
    {
        printf("error accepting\n");
    }
    chat(sockfd, newsockfd, client_address);
    close(sockfd);
    return 0;
}

void chat(int sockfd, int newsockfd, struct sockaddr_in client_address) 
{
    char buffer[BUFFER_SIZE];
    for (;;)
    {
        bzero(buffer, BUFFER_SIZE);
        if (recv(newsockfd, buffer, BUFFER_SIZE, 0) < 0)
        {
            printf("error reading from socket\n");
            return;
        }
        else
        {
            strtok(buffer, "\n");
            if (strcmp(buffer, "exit") == 0 || strlen(buffer) == 0)
            {
                printf("Received \"exit\" command from ip address %s, exiting\n", inet_ntoa(client_address.sin_addr));
                return;
            }
            printf("received message \"%s\" from ip address %s\n", buffer, inet_ntoa(client_address.sin_addr));
        }
        if (send(newsockfd, buffer, strlen(buffer), 0) < 0)
        {
            printf("error writing to socket\n");
        }
    }
}