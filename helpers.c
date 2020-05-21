#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "buffer.h"

#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)

void error(const char *msg) {
    perror(msg);
    exit(0);
}

void dealWithServer(int sockfd, char *message, char *response) {
    sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    close(sockfd);
    puts(response);           
    free(response);
    free(message);               
    
}

void makePromptUserPass(char *username, char *password) {
    printf("username=");
    scanf("%s", username);
    printf("password=");
    scanf("%s", password);
}

void makePromptBook(char *title, char *author, char *genre, char *publisher, char *page_count) {
    printf("title=");
    scanf("%s", title);
    printf("author=");
    scanf("%s", author);
    printf("genre=");
    scanf("%s", genre);
    printf("publisher=");
    scanf("%s", publisher);
    printf("page_count=");
    scanf("%s", page_count);
}

int checkBook(char *title, char *author, char *genre, char *publisher, char *page_count) {
    if (strcmp(title, "\n") == 0 || strlen(author) == 0 || strlen(genre) == 0
        || strlen(publisher) == 0 || strlen(page_count) == 0) {
            printf("Incomplete fields\n");
            return 0;
        }
    
    int pages = atoi(page_count);
    if (pages == 0) {
        printf("The number of pages must be an integer!\n");
        return 0;
    }

    if (pages < 0) {
        printf("The number of pages must be a positive number!\n");
        return 0;
    }

    return 1;
}

void compute_message(char *message, const char *line) {
    strcat(message, line);
    strcat(message, "\r\n");
}

int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag) {	
    struct sockaddr_in serv_addr;
    int sockfd = socket(ip_type, socket_type, flag);
    if (sockfd < 0)
        error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = ip_type;
    serv_addr.sin_port = htons(portno);
    inet_aton(host_ip, &serv_addr.sin_addr);
 

    /* connect the socket */
    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    return sockfd;
}

void close_connection(int sockfd) {
    close(sockfd);
}

void send_to_server(int sockfd, char *message) {
    int bytes, sent = 0;
    int total = strlen(message);

    do {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0) {
            error("ERROR writing message to socket");
        }

        if (bytes == 0) {
            break;
        }

        sent += bytes;
    } while (sent < total);
}

char *receive_from_server(int sockfd) {
    char response[BUFLEN];
    buffer buffer = buffer_init();
    int header_end = 0;
    int content_length = 0;

    do {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0){
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
        
        header_end = buffer_find(&buffer, HEADER_TERMINATOR, HEADER_TERMINATOR_SIZE);

        if (header_end >= 0) {
            header_end += HEADER_TERMINATOR_SIZE;
            
            int content_length_start = buffer_find_insensitive(&buffer, CONTENT_LENGTH, CONTENT_LENGTH_SIZE);
            
            if (content_length_start < 0) {
                continue;           
            }

            content_length_start += CONTENT_LENGTH_SIZE;
            content_length = strtol(buffer.data + content_length_start, NULL, 10);
            break;
        }
    } while (1);
    size_t total = content_length + (size_t) header_end;
    
    while (buffer.size < total) {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0) {
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
    }
    buffer_add(&buffer, "", 1);
    return buffer.data;
}

char *basic_extract_json_response(char *str) {
    return strstr(str, "{\"");
}
