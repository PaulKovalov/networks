#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

const int PORT = 8080;
const int BUFFER_SIZE = 131072;

int main()
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("error opening socket\n");
        return 0;
    }
    else
    {
        printf("success opening socket\n");
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("error connecting\n");
        return 0;
    }
    else
    {
        printf("succes connecting\n");
    }
    for (;;)
    {
        bzero(buffer, BUFFER_SIZE);
        printf("enter the operands:\n");
        fgets(buffer, BUFFER_SIZE, stdin);
        if (strcmp(buffer, "exit\n") == 0)
        {
            printf("Exiting...\n");
            break;
        }
        n = send(sockfd, buffer, strlen(buffer), 0);
        bzero(buffer, BUFFER_SIZE);
        printf("enter the operator:\n");
        fgets(buffer, BUFFER_SIZE, stdin);
        n = send(sockfd, buffer, strlen(buffer), 0);
        if (n < 0)
        {
            printf("error writing to socket\n");
            return 0;
        }
        bzero(buffer, BUFFER_SIZE);
        n = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if (n < 0)
        {
            printf("error reading from socket\n");
        }
        printf("Echoed from server: \"%s\"\n", buffer);
    }
    close(sockfd);
    return 0;
}