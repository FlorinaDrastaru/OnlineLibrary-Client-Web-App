#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"



char *compute_get_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count, char *token) {
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));

    // numele metodei, url,parametrii de request si tipul protocolului
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // adaug host-ul
    memset(line, 0,LINELEN);
    sprintf(line, "HOST: %s", host);
    compute_message(message, line);
    // adaug header si cookies
    if (cookies != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Cookie: ");
        for (int i = 0; i < cookies_count - 1; i++) {
            strcat(line, cookies[i]);
            strcat(line, ";");
        }
        strcat(line, cookies[cookies_count - 1]);
        compute_message(message, line);
    }
    // adaug token-ul in headerul Authorization
    if (token != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer ");
        strcat(line, token);
        compute_message(message, line);
    }
    compute_message(message, "");
    return message;
}

char *compute_delete_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count, char *token) {
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));

    // numele metodei, url,parametrii de request si tipul protocolului
    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // adaug host-ul
    memset(line, 0,LINELEN);
    sprintf(line, "HOST: %s", host);
    compute_message(message, line);
    // adaug headerele si cookie-urile
    if (cookies != NULL) {
       memset(line, 0, LINELEN);
       sprintf(line, "Cookie: ");
       for (int i = 0; i < cookies_count - 1; i++) {
        strcat(line, cookies[i]);
        strcat(line, ";");
       }
       strcat(line, cookies[cookies_count - 1]);
       compute_message(message, line);
    }
    // adaug tokenul in headerul Authorization
    if (token != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer ");
        strcat(line, token);
        compute_message(message, line);
    }
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char *content_type, char **body_data,
                            int body_data_fields_count, char **cookies, int cookies_count, char *token) {
    char* message = (char*)calloc(BUFLEN, sizeof(char));
    char* line = (char*)calloc(LINELEN, sizeof(char));

    // numele metodei, url,parametrii de request si tipul protocolului
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "HOST: %s", host);
    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Content-Length: %ld", strlen(body_data[0]));
    compute_message(message, line);

    // adaug cookie
    if (cookies != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Cookies: ");
        for(int i = 0; i < cookies_count-1; i++) {
            strcat(line, cookies[i]);
            strcat(line, ";");
        }
        strcat(line, cookies[cookies_count-1]);
        compute_message(message, line);
    }
 
   

    // adaug tokenul in headerul Authorization
    if (token != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer ");
        strcat(line, token);
        compute_message(message, line);
    }
    
    compute_message(message, "");
    compute_message(message, body_data[0]);

    free(line);
    return message;
}