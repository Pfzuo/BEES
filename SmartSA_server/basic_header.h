/*
 * basic header 
 * author: Jie Wu(courageJ)
*/

#include <sys/socket.h> // for socket 
#include <sys/types.h> // for socket 
#include <netinet/in.h>  // for sockaddr_in 
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define PORT 13320 // the port num
#define LISTENQ 128 // the length of listen queue
#define MAXLEN 1024 // the maximum length of buffer

#define IMAGE_DIR "./imagelib/" //the image library
#define ORBLIB_DIR "./orblib/" //the orb library
#define TMP_ORB_DIR "./tmporb/" //temporary orb files

const double kRed = 0.006; 
const double kCov = 0.8, kDiv = 0.2; //parameters of submodular subfunctions

#define RATIO 0.8 

#define TASK_DIRECT_UPLOAD 1
#define TASK_SmartSA_UPLOAD 2
