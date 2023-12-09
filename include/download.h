#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <regex.h>

#define MAX_LENGTH 512
#define SERVER_PORT 21

#define DEFAULT_USER "anonymous"
#define DEFAULT_PASSWORD "password"

#define AT "@"
#define BAR "/"
#define HOST_REGEX "%*[^/]//%[^/]"
#define HOST_AT_REGEX "%*[^/]//%*[^@]@%[^/]"
#define RESOURCE_REGEX "%*[^/]//%*[^/]/%s"
#define USER_REGEX "%*[^/]//%[^:/]"
#define PASS_REGEX "%*[^/]//%*[^:]:%[^@\n$]"
#define RESPCODE_REGEX "%d"
#define PASSIVE_REGEX "%*[^(](%d,%d,%d,%d,%d,%d)%*[^\n$)]"


//server responses
#define SERVER_READY_FOR_TRANSFER 150
#define SERVER_READY_FOR_USER 220
#define SERVER_GOODBYE 221
#define SERVER_TRANSFER_COMPLETE 226
#define SERVER_ENTERING_PASSIVE_MODE 227
#define SERVER_LOGIN_SUCCESSFUL 230
#define SERVER_NEED_PASSWORD  331


struct URL {
    char host[MAX_LENGTH];
    char resource[MAX_LENGTH];
    char file[MAX_LENGTH];
    char user[MAX_LENGTH];
    char password[MAX_LENGTH];
    char ip[MAX_LENGTH];
};

typedef enum {
    START,
    SINGLE,
    MULTIPLE,
    END
} ResponseState;

void printURLDetails(const struct URL *url);
void parseURL(const char *urlString, struct URL *url);
int createSocket(char *ip, int port);
int readResponse(const int socket, char *buffer);
void handleStartState(const char byte, ResponseState *state, char *buffer, int *index);
void handleSingleState(const char byte, ResponseState *state, char *buffer, int *index);
void handleMultipleState(const char byte, ResponseState *state, char *buffer, int *index);
void passiveMode(const int socket, char *ip, int *port);
void sendRetrievalCommand(const int socket, const char *resource);
void requestFile(const int socket, const char *resource);
void getFile(const int socketA, const int socketB, char *filename);
void closeConnection(const int socketA, const int socketB);

#endif /* DOWNLOAD_H */