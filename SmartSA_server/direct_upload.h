/*
 * direct upload strategy of images from the client
 * author: Jie Wu(courageJ)
*/

#include <iostream>
#include <cstdio>
 #include <string>
using namespace std;

string GetFileName(); // name new files based on the current time
ssize_t Readn(int fd, void *vptr, size_t n); // read the data stream from the client
void WriteImagetoFile(int connfd) ; // receive th data stream of images and save

#define Default_Orb_File 16000 