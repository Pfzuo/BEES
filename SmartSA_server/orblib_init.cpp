/*
 * orblib init
 * author: Jie Wu(courageJ)
*/

#include <iostream>
#include "basic_header.h"
#include "orb.h"
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <dirent.h>
using namespace std;


int main(int argc, const char* argv[]) {
    // input image dir
    // output->orblib
    string imgDir = string(argv[1]);
    string orbDir = ORBLIB_DIR;
    string imgName;

    cv::Mat image;
    vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    cv::Ptr<cv::FeatureDetector> detector;
    cv::Ptr<cv::DescriptorExtractor> extractor;
    detector = cv::FeatureDetector::create("ORB");  
    extractor = cv::DescriptorExtractor::create("ORB");

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(imgDir.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            string name = (string)(ent -> d_name);
            if (has_suffix(name, "g")) {
                //jpg, png, jpeg, etc
                imgName = imgDir + name;
                image = cv::imread(imgName, 1);
                detector -> detect(image, keypoints);
                extractor -> compute(image, keypoints, descriptors);
                string outfile = orbDir + name +".xml";
                orb_write(descriptors, outfile);
            }
        }
    }
    else {
        std::cout <<"Error: can't open the directory!" << std::endl;
        exit(-1);
    }
    return 0;
}
