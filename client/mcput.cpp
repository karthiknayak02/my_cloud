#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>

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
    char filename[80];
    uint32_t size;
    char filedata[100*1024];
} request;

typedef struct response_struct {
    int status;
} response;

int main(int argc, char **argv)
{
    int clientfd, port;         // client file descriptor and port number
    char *host;
    struct stat st;             // for file size
    rio_t rio;                  // rio for TCP sockets
    request req;                // request struct from client
    response resp;              // response struct from server

    if (argc != 5) { fprintf(stderr, "usage: %s <MachineName> <TCPport> <SecretKey> <filename>\n", argv[0]); exit(0); }
    
    host = argv[1];
    port = atoi(argv[2]);
    req.secret = htonl(atoi(argv[3]));
    strcpy(req.filename, argv[4]);
    
    req.type = htonl(1);
    
    FILE * fileToSend;
    fileToSend = fopen (req.filename, "rb");
    
    if(fileToSend == NULL || stat(req.filename, &st) != 0) error("Error opening file");
        
    else if (st.st_size > 102400) error("File exceeds 100KB");
                                        
    else {
        req.size = htonl(st.st_size);
        if (fread (req.filedata,  sizeof(char), st.st_size, fileToSend) != st.st_size)
            error("Error reading file");
            
        fclose (fileToSend);
    }
    
    clientfd = Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);
    
    Rio_writen(clientfd, &req, 92 + st.st_size);
                                        
    //-----------Recieve Response-----------
    Rio_readn(clientfd, &resp.status, 4);
    
    if (ntohl(resp.status) == 0) cout << "Success" << endl;
    else cout << "Error" << endl;
    
    Close(clientfd);
    exit(0);
}


