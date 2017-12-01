#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>

extern "C" {
#include "csapp.h"
}

using namespace std;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

typedef struct request_struct {
    uint32_t secret;
    uint32_t type;
} request;

typedef struct response_struct {
    int status;
    uint32_t size;
} response;

int main(int argc, char **argv)
{
    int clientfd, port;         // client file descriptor and port number
    char *host;
    rio_t rio;                  // rio for TCP sockets
    request req;                // request struct from client
    response resp;              // response struct from server
    
    if (argc != 4) { fprintf(stderr, "usage: %s <MachineName> <TCPport> <SecretKey>\n", argv[0]); exit(0); }
    
    host = argv[1];
    port = atoi(argv[2]);
    req.secret = htonl(atoi(argv[3]));
    
    req.type = htonl(3);
    
    clientfd = Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);
    
    Rio_writen(clientfd, &req, 8);
    
    //-----------Recieve Response-----------
    Rio_readn(clientfd, &resp.status, 4);
    
    if (ntohl(resp.status) == 0) {
        Rio_readn(clientfd, &resp.size, 4);
        for(int i = 0; i < ntohl(resp.size); i++) {
            char buf[80];
            Rio_readn(clientfd, &buf, 80);
            cout << i << "] "<< buf << endl;
        }
    }
    else cout << "Error" << endl;
    
    Close(clientfd);
    exit(0);
}


