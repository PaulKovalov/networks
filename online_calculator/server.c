#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

const int BUFFER_SIZE = 131072;
const int PORT = 8080;

// node of the linked list

struct Node
{
    struct Node *next;
    char *data;
};

void chat(int, int, struct sockaddr_in);

char *concat(struct Node *);
int add(struct Node *);
int mul(struct Node *);
struct Node *parse_input(char *, int *);
char *int_to_char(int);

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
    char command[BUFFER_SIZE];
    char *result;
    for (;;)
    {
        bzero(buffer, BUFFER_SIZE);
        bzero(command, BUFFER_SIZE);
        int read_operands = recv(newsockfd, buffer, BUFFER_SIZE, 0);
        int read_operator = recv(newsockfd, command, BUFFER_SIZE, 0);
        if (read_operands < 0 || read_operator < 0)
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
            strtok(command, "\n");
            printf("received command \"%s\" with params \"%s\" from ip address %s\n", command, buffer, inet_ntoa(client_address.sin_addr));

            int numbers = 0;
            struct Node *list = parse_input(buffer, &numbers);
            if (strcmp(command, "mul") == 0)
            {
                if (!numbers)
                {
                    result = "Error, cannot multiply strings";
                    send(newsockfd, result, strlen(result), 0);
                    break;
                }
                result = int_to_char(mul(list));
                printf("Multiplying numbers: %s\n", result);
            }
            else if (strcmp(command, "add") == 0)
            {
                if (!numbers)
                {
                    result = concat(list);
                    printf("Concatenating strings: %s\n", result);
                }
                else
                {
                    result = int_to_char(add(list));
                    printf("Adding numbers: %s \n", result);
                }
            }
        }
        if (send(newsockfd, result, strlen(result), 0) < 0)
        {
            printf("error writing to socket\n");
        }
    }
}

struct Node *parse_input(char *buffer, int *numbers)
{
    *numbers = 1;
    struct Node *head = (struct Node *)malloc(sizeof(struct Node)), *curr = (struct Node *)malloc(sizeof(struct Node));
    char *pch;
    pch = strtok(buffer, " ");
    head->data = pch;
    curr = head;
    while (pch != NULL)
    {
        pch = strtok(NULL, " ");
        if (pch != NULL)
        {
            curr->next = (struct Node *)malloc(sizeof(struct Node));
            curr->next->data = pch;
            curr = curr->next;
            int minus_before = 0;
            for (int i = 0; i < strlen(pch); ++i)
            {
                if (pch[i] == '-')
                {
                    if (minus_before)
                    {
                        *numbers = 0;
                    }
                    else
                    {
                        minus_before = 1;
                    }
                }
                else if (!isdigit(pch[i]))
                {
                    *numbers = 0;
                }
                else
                {
                    minus_before = 0;
                }
            }
        }
    }
    return head;
}

char *concat(struct Node *list)
{
    int total_length = 0;
    for (struct Node *curr = list; curr != NULL; curr = curr->next)
        total_length += strlen(curr->data);
    char *result = malloc(total_length + 2);
    for (struct Node *curr = list; curr != NULL; curr = curr->next)
    {
        strcat(result, curr->data);
    }
    return result;
}

int add(struct Node *list)
{
    int sum = 0;
    for (struct Node *curr = list; curr != NULL; curr = curr->next)
        sum += atoi(curr->data);
    return sum;
}

int mul(struct Node *list)
{
    int res = 1;
    for (struct Node *curr = list; curr != NULL; curr = curr->next)
        res *= atoi(curr->data);
    return res;
}

char *int_to_char(int n)
{
    char buff[100];
    sprintf(buff, "%d", n);
    buff[strlen(buff)] = '\0';
    char *str = malloc(strlen(buff) + 1);
    strcpy(str, buff);
}