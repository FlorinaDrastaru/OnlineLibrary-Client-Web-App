#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include "parson.h"
#include "helpers.h"
#include "requests.h"



int main() {
    int sockfd;
    char *server_address = "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com";
    char *registerApi = "/api/v1/tema/auth/register";
    char *login = "/api/v1/tema/auth/login";
    char *library = "/api/v1/tema/library/access";
    char *books = "/api/v1/tema/library/books";
    char *logout = "/api/v1/tema/auth/logout";
    char *JsonApp = "application/json";
    char *cookie = calloc(400, sizeof(char));
    char *token = calloc(400, sizeof(char));
    strcpy(cookie, "");
    strcpy(token, "");
    char *message;
    char *response;
    char *command = calloc(40, sizeof(char));

    // cat timp citesc de la tastatura
    while (1) {
        // citesc comanda
        scanf("%s", command);
        if (strcmp(command, "exit") == 0) {
            break;
        } else if (strcmp(command, "register") == 0) {  
            char *username = calloc(20, sizeof(char));
            char *password = calloc(20, sizeof(char));
            // prompt pentru user si parola
            makePromptUserPass(username, password);
            // parsare json
            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            json_object_set_string(root_object, "username", username);
            json_object_set_string(root_object, "password", password);
            char *json = json_serialize_to_string_pretty(root_value);
            message = compute_post_request(server_address, registerApi, JsonApp, &json, 1, NULL, 0, NULL);
            dealWithServer(sockfd, message, response);
        } else if (strcmp(command, "login") == 0) {
            if (strlen(cookie) != 0) {
                printf("Another user is authenicated!\n");
            } else {
                char *username = calloc(20, sizeof(char));
                char *password = calloc(20, sizeof(char));
                char *json;
                // prompt pentru user si parola
                makePromptUserPass(username, password);
                // parsare json
                JSON_Value *root_value = json_value_init_object();
                JSON_Object *root_object = json_value_get_object(root_value);
                json_object_set_string(root_object, "username", username);
                json_object_set_string(root_object, "password", password);
                json = json_serialize_to_string_pretty(root_value);
                message = compute_post_request(server_address, login, JsonApp, &json, 1, NULL, 0, NULL);
                sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                puts(response);
                char *copy_response = strdup(response);
                free(response);
                free(message);
                close(sockfd);
                // caut in raspunsul de la server cookie-ul si il salvez
                char *startOfString = strstr(copy_response, "connect");
                if (startOfString != NULL) {
                    char *endOfString = strchr(startOfString, ';');
                    int size = endOfString - startOfString;
                    strncpy(cookie, startOfString, size);
                    printf("You successfully logged in!\n");
                }
                free(copy_response);
            }
        } else if (strcmp(command, "enter_library") == 0) {
            // verific daca userul e autentificat
            if (strlen(cookie) == 0) {
                printf("You are not authenticated!\n");
            } else {
                message = compute_get_request(server_address, library, NULL, &cookie, 1, NULL);
                sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                // caut inceputul token-ului        
                char *startOfString = strstr(response, "{");
                // Convertesc token-ul din format JWT in string
                JSON_Value *tokVal = json_parse_string(startOfString);
                JSON_Object *tokObj = json_value_get_object(tokVal);
                const char *tok = json_object_get_string(tokObj, "token");
                // salvez token-ul pentru a-l folosi la diverse actiuni din biblioteca      
                strcpy(token, tok);
                puts(response);
                free(response);
                free(message);               
                close(sockfd);
            }
        } else if (strcmp(command, "get_books") == 0) {
            // verific accesul
            if (strlen(token) == 0) {
                printf("You don't have access to library!\n");
            } else {
                message = compute_get_request(server_address, books, NULL, &cookie, 1, token);           
                dealWithServer(sockfd, message, response);
            }            
        } else if (strcmp(command, "get_book") == 0) {
            // verific accesul
            if (strlen(token) == 0) {
                printf("You don't have access to library!\n");
            } else {
                char *bookId = calloc(20, sizeof(char));
                printf("id=");
                scanf("%s", bookId);
                // obtin noua adresa url in functie de id-ul cartii         
                char *newBooks = strdup(books);
                strcat(newBooks, "/");      
                strcat(newBooks, bookId);            
                message = compute_get_request(server_address, newBooks, NULL, &cookie, 1, token);           
                dealWithServer(sockfd, message, response);
            }
        } else if (strcmp(command, "add_book") == 0) {  
            if (strlen(token) == 0) {
                printf("You don't have access to library!\n");
            } else {                      
                char *title = calloc(20, sizeof(char));
                char *author = calloc(20, sizeof(char));
                char *genre = calloc(20, sizeof(char));
                char *publisher = calloc(20, sizeof(char));
                char *page_count = calloc(20, sizeof(char));
                // prompt pentru detaliile despre carte
                makePromptBook(title, author, genre, publisher, page_count);
                    if (checkBook(title, author, genre, publisher, page_count)) {
                    JSON_Value *root_value = json_value_init_object();
                    JSON_Object *root_object = json_value_get_object(root_value);
                    json_object_set_string(root_object, "title", title);
                    json_object_set_string(root_object, "author", author);
                    json_object_set_string(root_object, "genre", genre);
                    json_object_set_string(root_object, "publisher", publisher);
                    json_object_set_string(root_object, "page_count", page_count);
                    char *json = json_serialize_to_string_pretty(root_value);          
                    message = compute_post_request(server_address, books, JsonApp, &json, 1, &cookie, 1, token);      
                    dealWithServer(sockfd, message, response);
                    printf("The book was added!\n");
                }
            }
        } else if (strcmp(command, "delete_book") == 0) {
            // verific accesul
            if (strlen(token) == 0) {
                printf("You don't have access to library!\n");
            } else {
                char *bookId = calloc(20, sizeof(char));
                printf("id=");
                scanf("%s", bookId);
                // creez noua adresa url, in functie de id-ul cartii    
                char *newBooks = strdup(books);
                strcat(newBooks, "/");
                strcat(newBooks, bookId);
                message = compute_delete_request(server_address, newBooks, NULL, &cookie, 1, token);
                dealWithServer(sockfd, message, response);
            }
        } else if (strcmp(command, "logout") == 0) {
            if (strlen(cookie) == 0) {
                printf("You can't logout! You are not authenticated!\n");
            } else {
                message = compute_get_request(server_address, logout, NULL, &cookie, 1, token); 
                dealWithServer(sockfd, message, response);
                printf("You successfully logged out!\n");  
                // resetez cookie
                if (strlen(cookie) != 0) {     
                    strcpy(cookie, "");
                }
                // resetez token          
                if (strlen(token) != 0) {
                    strcpy(token, "");
                }
            }        
        }       
    } 
}

