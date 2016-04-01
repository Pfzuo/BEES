/*
 * net server
 * author: Jie Wu(courageJ)
*/
#include <iostream>
#include <string.h> 
#include "config.h"
#include "direct_upload.h"
#include "SmartSA_upload.h"
#include "orb.h"
#include <cstdio>

using namespace std;

// network init
int open_listenfd(int port) 
{
    int listenfd, optval=1;
    struct sockaddr_in server_addr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }

    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                (const void *)&optval, sizeof(int)) < 0) {
        return -1;
    }

    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons((unsigned short)port);
    
    if (bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        return -1;
    }

    if (listen(listenfd, LISTENQ) < 0) {
        return -1;
    }
    return listenfd;
}

// send data stream to client
ssize_t Writen(int fd, const void *vptr, ssize_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;
    
    ptr = (char *)vptr;
    nleft = n;
    while(nleft > 0)
    {
        if((nwritten = write(fd, ptr, nleft)) <= 0)
        {
            if(nwritten < 0 && errno == EINTR)
                nwritten = 0;
            else return -1;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}

//Direct upload strategy
void DirectUpload(int connfd) {
    cout <<"DirectUpload strategy"<<endl;
    WriteImagetoFile(connfd);
    cout<<"DirectUpload done!" << endl;
}

//SmartSAUpload strategy
void SmartSAUpload(int connfd) {

    cout << "SmartSAUpload strategy"<<endl;
    SmartSAinit();
    WriteOrbtoFile(connfd);
    string ret = GetUploadFile(); // return 01 string stands for image sequence 
    int buf;
    for (int i = 0; i < ret.length(); i++) {
        if (ret[i] == '0') {
            buf = 0;
        }
        else {
            buf = 1;
        }
        Writen(connfd, &buf, sizeof(int));
    }
    WriteImagetoFile(connfd);
    cout << "SmartSAUpload done!" << endl;
}


//receive the task type and do next step
void Rec (int connfd) {
    /*
    receive the message from the client and do some transformation
    task_type = TASK_DIRECT_UPLOAD: imageamount||image[1]size||image[1]||image[2]size||image[2]||...||image[imageamount]size||image[imageamount]
    task_type = TASK_SmartSA_UPLOAD: BatteryEnergy||orbamount||orb[1]size||orb[1]||...||orb[orbamount]size||orb[orbamount]
    */
    int task_type; //upload strategy. 1 for 'direct upload', 2 for 'SmartSA'.
    char buf[MAXLEN]; // receive message buffer

    // identity the task type
    memset(buf, '\0', sizeof(buf));
    read(connfd, buf, sizeof(int));
    task_type = *(int *)buf; 
    switch(task_type) {
        case TASK_DIRECT_UPLOAD: DirectUpload(connfd); break;
        case TASK_SmartSA_UPLOAD: SmartSAUpload(connfd); break;
        default: cout <<"strategy error";
    }
    //cout <<"Rec done!" <<endl;
}

int main(int argc, char *argv[]) {
    struct sockaddr_in client_addr; 
    int listenfd, connfd;
    socklen_t client_len;

    /*Create a listen descriptor*/
    listenfd = open_listenfd(PORT);
    while (1) {
        client_len = sizeof(client_addr);
        if ((connfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            cout << "Accept failed!" <<endl;
            break;
        }
        cout <<"Server connected to (IP: " <<inet_ntoa(client_addr.sin_addr)<<"), (port: "<<ntohs(client_addr.sin_port)<<")"<<endl;
        Rec(connfd);
    }
    close(connfd);
    close(listenfd);
    return 0;
}


