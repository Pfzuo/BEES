/*
 * the implementation for the functions in direct upload.h
 * author: Jie Wu(courageJ)
*/

#include "direct_upload.h"
#include "basic_header.h"
#include "orb.h"

#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <iterator>  
#include <dirent.h>
#include <fstream>

using namespace std;

// A function to name new files based on the current time
string GetFileName () {
    struct timeval tv;
    struct tm *curtime;
    
    gettimeofday(&tv, NULL);
    curtime = localtime(&tv.tv_sec);
    char chfilename[256] = "";
    sprintf(chfilename, "%04d%02d%02d%02d%02d%02d%d", curtime->tm_year+1900, 
        curtime->tm_mon+1, curtime->tm_mday, curtime->tm_hour, curtime->tm_min,
        curtime->tm_sec, (int)tv.tv_usec);
    string strfilename = chfilename;

    return strfilename;
}

// Read the data stream from the client
ssize_t Readn(int fd, void *vptr, size_t n){
    size_t nleft;
    ssize_t nread;
    char *ptr;

    ptr = (char *)vptr;
    nleft = n;
    while(nleft > 0) {
        if((nread = read(fd, ptr, nleft)) < 0) {
            if(errno == EINTR)
                nread = 0;
            else return -1;
        }
        else if(nread == 0)
            break;
        nleft -= nread;
        ptr += nread;
    }
    return n - nleft;
}

// Receive th data stream of images and save
void WriteImagetoFile(int connfd) {
    char buf[MAXLEN];
    memset(buf, '\0', sizeof(buf));
    read(connfd, buf, sizeof(int));
    int imageamount = *(int *)buf;
    cout <<"upload images amount = "<< imageamount << endl;
    for (int id = 0; id < imageamount; id++) {
        int imagesize = 0;
        memset(buf, '\0', sizeof(buf));
        read(connfd, buf, sizeof(int));
        imagesize = *(int *) buf;

        char *content;
        content = (char *)malloc(imagesize * sizeof(char));

        string filename = GetFileName();
        FILE *fp;
        string imagetype = ".jpg";
        filename += imagetype;
        fp = fopen((IMAGE_DIR+filename).c_str(), "w+");
        const int IMAGE_SIZE = 1024;
        int nwritten = 0, n;
        while(nwritten < imagesize) {
            if (imagesize - nwritten < IMAGE_SIZE) {
                n = Readn(connfd, content, imagesize - nwritten);
            }
            else {
                n = Readn(connfd, content, IMAGE_SIZE);
            }
            nwritten += n;
            if(fwrite(content, sizeof(char), n, fp) != n){
                fprintf(stderr, "writeImageToFile: fwrite failed\n");
            }  
        }

        free(content);
        fclose(fp);
        GetOrbofImage(IMAGE_DIR, filename);
        // next: extract the orb of image and save in the orblib;
    }
}




