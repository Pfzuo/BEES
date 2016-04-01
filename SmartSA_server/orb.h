/*
 * extract orb information of images and do some related match
 * author: Jie Wu(courageJ)
*/

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp> 
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iomanip> 
#include <string>

using namespace std;

void orb_write(cv::Mat descriptors, string output_file); // save orb in files
void GetOrbofImage(string imagedir, string imagename); // generate orb of images
cv::Mat orb_read(string input_file); // read orb from files
bool has_suffix(const std::string &str, const std::string &suffix); // check whether the string has the suffix
vector<cv::DMatch> ratio_test(vector< vector<cv::DMatch> > matches12, double ratio); //ratio test nearest/second nearest < ratio
vector<cv::DMatch> symmetric_test(std::vector<cv::DMatch> good_matches1, std::vector<cv::DMatch> good_matches2); // symmetric Test
double ImageSearch(string queryimageorbdir, string queryimageorbname); //search orblib to find the max score of image
double GetScoreBtwImages(string srcOrbDir, string srcOrbfilename, string desOrbDir, string desOrbfilename);//get the score between images
 