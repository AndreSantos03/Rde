#include "../include/download.h"

void printURLDetails(const struct URL *url) {
    printf("Host: %s\n", url->host);
    printf("Resource: %s\n", url->resource);
    printf("File: %s\n", url->file);
    printf("User: %s\n", url->user);
    printf("Password: %s\n", url->password);
    printf("IP: %s\n", url->ip);
}

void parseURL(const char *urlString, struct URL *url) {
    
    regex_t regex;
    regcomp(&regex, BAR, 0);
    if (regexec(&regex, urlString, 0, NULL, 0)) {
        printf("wrong url format");
        regfree(&regex);
        return;
    }

    regcomp(&regex, AT, 0);
    if (regexec(&regex, urlString, 0, NULL, 0) != 0) { // for ftp://<host>/<url-path>
        
        sscanf(urlString, HOST_REGEX, url->host);
        strcpy(url->user, DEFAULT_USER);
        strcpy(url->password, DEFAULT_PASSWORD);

    } else { // for ftp://[<user>:<password>@]<host>/<url-path>

        sscanf(urlString, HOST_AT_REGEX, url->host);
        sscanf(urlString, USER_REGEX, url->user);
        sscanf(urlString, PASS_REGEX, url->password);
    }

    sscanf(urlString, RESOURCE_REGEX, url->resource);
    strcpy(url->file, strrchr(urlString, '/') + 1);

    struct hostent *h;
    if (strlen(url->host) == 0) {
        printf("Invalid hostname '%s'\n", url->host);
        return; 
    }

    //code from getip.c
    if ((h = gethostbyname(url->host)) == NULL) {
        printf("Failed to resolve hostname '%s'\n", url->host);
        return; 
    }


    strcpy(url->ip, inet_ntoa(*((struct in_addr *)h->h_addr)));
    regfree(&regex);
}

int createSocket(char *ip, int port){
    //code from clientTCP.c
    int sockfd;
    struct sockaddr_in server_addr;

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);    /*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(port);        /*server TCP port must be network byte ordered */

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("Socket creation failed.\n");
            exit(-1);
        }
        /*connect to the server*/
        if (connect(sockfd,(struct sockaddr *) &server_addr,sizeof(server_addr)) < 0) {
            printf("Connection failed.\n");
            exit(-1);
    }

    return sockfd;
}

int readResponse(const int socket, char* buffer) {
    char byte;
    int index = 0;
    int responseCode;
    ResponseState state = START;

    memset(buffer, 0, MAX_LENGTH);

    while (state != END) {
        read(socket, &byte, 1);
        switch (state) {
            case START:
                handleStartState(byte, &state, buffer, &index);
                break;
            case SINGLE:
                handleSingleState(byte, &state, buffer, &index);
                break;
            case MULTIPLE:
                handleMultipleState(byte, &state, buffer, &index);
                break;
            case END:
                break;
            default:
                break;
        }
    }
    sscanf(buffer, RESPCODE_REGEX, &responseCode);
    printf("response code: %d\n", responseCode);
    return responseCode;
}

void handleStartState(const char byte, ResponseState* state, char* buffer, int* index) {
    if (byte == ' ') {
        *state = SINGLE;
    } else if (byte == '-') {
        *state = MULTIPLE;
    } else if (byte == '\n') {
        *state = END;
    } else {
        buffer[(*index)++] = byte;
    }
}

void handleSingleState(const char byte, ResponseState* state, char* buffer, int* index) {
    if (byte == '\n') {
        *state = END;
    } else {
        buffer[(*index)++] = byte;
    }
}

void handleMultipleState(const char byte, ResponseState* state, char* buffer, int* index) {
    if (byte == '\n') {
        memset(buffer, 0, MAX_LENGTH);
        *state = START;
        *index = 0;
    } else {
        buffer[(*index)++] = byte;
    }
}

int authenticateServer(const int socket, const char* user, const char* pass) {
    char userCommand[MAX_LENGTH];
    sprintf(userCommand, "user %s\n", user);

    char passCommand[MAX_LENGTH];
    sprintf(passCommand, "pass %s\n", pass);

    char answer[MAX_LENGTH];

    char authCommands[MAX_LENGTH * 2];
    sprintf(authCommands, "%s%s", userCommand, passCommand);

    write(socket, authCommands, strlen(authCommands));

    int response = readResponse(socket, answer);
    if (response != SERVER_NEED_PASSWORD) {
        printf("Unknown user '%s'. Abort.\n", user);
        exit(-1);
    }

    return readResponse(socket, answer);
}


void passiveMode(const int socket, char *ip, int *port) {
    char response[MAX_LENGTH];
    write(socket, "pasv\n", 5);
    if (readResponse(socket, response) != SERVER_ENTERING_PASSIVE_MODE){
        printf("Failed entering passive mode!");
        exit(-1);
    }

    int ip1, ip2, ip3, ip4;
    int port1, port2;

    if (sscanf(response, PASSIVE_REGEX, &ip1, &ip2, &ip3, &ip4, &port1, &port2) != 6) {
        printf("Error parsing response for passive mode\n");
        exit(-1);
    }
    
    *port = port1 * 256 + port2;//combines the two ports using mathematical formula

    sprintf(ip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
}

void sendRetrievalCommand(const int socket, const char *resource) {
    char fileCommand[MAX_LENGTH];
    snprintf(fileCommand, sizeof(fileCommand), "retr %s\n", resource);
    write(socket, fileCommand, strlen(fileCommand));
}

void requestFile(const int socket, const char *resource) {
    sendRetrievalCommand(socket, resource);
    char answer[MAX_LENGTH];
    printf("goat\n");
    int responseCode = readResponse(socket, answer);
    if(responseCode != SERVER_READY_FOR_TRANSFER){
        printf("Requested resource does not exist!");
        exit(-1);
    }
}



int main(int argc, char **argv){
    if(argc != 2){
        printf("You must enter 2 arguments!");
        return -1;
    }
    struct URL url;
    parseURL(argv[1], &url);
    int socketOne = createSocket(url.ip,SERVER_PORT);

    char response[MAX_LENGTH];
    int responseCode = readResponse(socketOne, response);
    if(responseCode != SERVER_READY_FOR_USER){
        printURLDetails(&url);
        printf("Connection Failed! \n");
    }

    int authResponse = authenticateServer(socketOne, url.user, url.password);
    if(authResponse != SERVER_LOGIN_SUCCESSFUL){
        printURLDetails(&url);
        printf("Failed to authenticate!\n");
    }

    int port;
    char ip[40];
    passiveMode(socketOne, ip, &port);

/*     int socketTwo = createSocket(ip,port);
 */
    requestFile(socketOne, url.resource);
    printf("url resource: %s", url.resource);

    return 0;
}
