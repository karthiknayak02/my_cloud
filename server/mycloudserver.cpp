#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>

extern "C" {
    #include "csapp.h"
}

using namespace std;

typedef struct request_struct {
    uint32_t type;
    uint32_t secret;
    char filename[80];
    uint32_t size;
    char filedata[100*1024];
} request;

typedef struct response_struct {
    int status;
    uint32_t size;
    char data[100*1024];
} response;

string typeToString(int num) {
    switch (num) {
        case 0:
            return  "cget";
        case 1:
            return  "cput";
        case 2:
            return  "cdelete";
        case 3:
            return  "clist";
        default:
            return "quit";
    }
}

int req_byte(string name, uint32_t size) {
    if (size > 102400) cerr << "Invalid size" << endl;
    else if (name == "cget")      return  8 + size;
    else if (name == "cput")      return  4;
    else if (name == "cdelete")   return  4;
    return  0;
}

int main(int argc, char **argv)
{
    int listenfd, connfd, port, status = -1, test = 1;
    uint32_t secret;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    char client_hostname[MAXLINE], client_port[MAXLINE];
    unsigned char isFile = 0x8;
    string req_type, list_line;
    request req;                // request struct from client
    response resp;              // response struct from server
    struct stat st;             // for file size
    char buf[80];
    DIR *dir;
    struct dirent *dirEntry;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <TCPport> <SecretKey>\n", argv[0]);
        exit(0);
    }
    
    port = atoi(argv[1]);
    secret = atoi(argv[2]);
    
    listenfd = Open_listenfd(port);
    cout << "Server socket created..." << endl;

    clientlen = sizeof(struct sockaddr_storage);
    
    while ((connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen))) {
        getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE,
                    client_port, MAXLINE, 0);
        printf("Connection accepted from (%s, %s)\n", client_hostname, client_port);
        

        //-----------Recieve Request-----------
        Rio_readn(connfd, &req.secret, 4);
        Rio_readn(connfd, &req.type, 4);

        req_type = typeToString(ntohl(req.type));
        if (req_type != "clist") Rio_readn(connfd, &req.filename, 80);
        if (req_type == "cput") {
            Rio_readn(connfd, &req.size, 4);
            Rio_readn(connfd, &req.filedata, ntohl(req.size));
        }
        
        //cout << "Server recieved request..." << endl;
        
        //-----------Make Response Struct-----------
        if (secret == ntohl(req.secret) && req_type != "quit") {
            status = 0;
            if (req_type == "cput") {
                cout << "Inside cput\n";
                FILE * fileToReceive;
                fileToReceive = fopen (req.filename, "wb");
                fwrite (req.filedata, sizeof(char), ntohl(req.size), fileToReceive);
                fclose (fileToReceive);
                resp.status = htonl(status);
            }
            
            else if (req_type == "cget") {
                FILE * fileToSend;
                fileToSend = fopen (req.filename, "rb");
                
                if(fileToSend == NULL || stat(req.filename, &st) != 0) {
                    cerr << "Error opening file" << endl; status = -1; }
                else if (st.st_size > 102400) {
                    cerr << "File exceeds 100KB" << endl; status = -1; }
                else {
                    resp.size = htonl(st.st_size);
                    if (fread (resp.data,  sizeof(char), st.st_size, fileToSend) != st.st_size) {
                        cerr << "Error reading file" << endl; status = -1; }
                    fclose (fileToSend);
                }
                resp.status = htonl(status);
            }
            
            else if (req_type == "cdelete" && remove (req.filename) != 0) {
                resp.status = htonl(-1); status = -1; }
            
            else if (req_type == "clist") {
                dir = opendir (".");
                vector<string> wdLines;
                
                resp.status = htonl(status);
                Rio_writen(connfd, &resp.status, 4);
                while ((dirEntry = readdir (dir))) if (dirEntry->d_type == isFile) {
                    wdLines.push_back(dirEntry->d_name);
                }
                closedir (dir);
                
                resp.size = htonl(wdLines.size());
                Rio_writen(connfd, &resp.size, 4);
                
                for(int i = 0; i < wdLines.size(); i++) {
                    strncpy(buf, wdLines[i].c_str(), 80);
                    buf[strlen(wdLines[i].c_str())] = '\0';
                    Rio_writen(connfd, buf, 80);
                }
            }
            
        }
        else if (req_type != "quit") {
            resp.status = htonl(-2);
            if (req_type == "clist") Rio_writen(connfd, &resp.status, 4);
        }
        
        //cout << "Server created response..." << endl;
        
        //-----------Send Response-----------
        if (req_type != "clist") Rio_writen(connfd, &resp, req_byte(req_type, st.st_size));
        
        if (secret == ntohl(req.secret)) {
            cout << "-------------------------------------------"   << endl;
            cout << "Request Type       = " << req_type             << endl;
            cout << "Secret Key         = " << ntohl(req.secret)    << endl;
            cout << "Filename           = " << req.filename         << endl;
            cout << "Operation Status   = " << resp.status          << endl;
            cout << "-------------------------------------------"   << endl;
        }
        else cout << "Incorrect Secret Key" << endl;
        
        resp = {};
        req = {};
        test++;
    }
    
    Close(connfd);
    cout << "Server socket closed." << endl;
    exit(0);
}
