/*
 * SmartSA upload strategy for images from the client
 * author: Jie Wu(courageJ)
*/
#include <string>
using namespace std;
void tarjan(int u);  //generate the strong connected  components
void SmartSAinit(); //SmartSA Init
void WriteOrbtoFile(int connfd); //save orb in temp folder
void GenSubGroup(); //count the score based on battery, and select file over the score
void CalcWeight(); // calculate redundancy w(i, j) between images
void CalcUploadAmount(); //calculate number of images should be uploaded
double Coverage(); //submodular subfunction: Coverage
int Diversity(); //submodular subfunction: diversity
double Submodular_Func(); //submodular function
string submodular(); //return id of images to be uploaded based on submodular function
string GetUploadFile(); //get the files id

