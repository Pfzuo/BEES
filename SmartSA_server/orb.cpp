/*
 * the implementation for the functions in orb.h
 * author: Jie Wu(courageJ)
*/

#include "basic_header.h"
#include "orb.h"
#include <string>
#include <dirent.h>
#include <queue>
#include <vector>
#include <algorithm>

using namespace std;

struct  MatchScore{
    string query_image_name;
    string train_image_name;
    int query_image_keypoints_size;
    int train_image_keypoints_size;
    int good_matches_size;
    double jaccard;
};

struct  ScoreComp{
    bool operator()(MatchScore &a, MatchScore &b){
        return a.jaccard < b.jaccard;
    }
}comp;

// save orb in files
void orb_write(cv::Mat descriptors, string output_file){
    cv::FileStorage outfile(output_file, cv::FileStorage::WRITE );
    outfile << "descriptor" << descriptors;
    outfile.release();
}


//extract orb of image (image name has the file type(.jpg))
void GetOrbofImage(string imagedir, string imagename) {
    string orbdir = ORBLIB_DIR;

    cv::Mat image = cv::imread(imagedir+imagename, 1);
    vector <cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    cv::Ptr<cv::FeatureDetector> detector;
    cv::Ptr<cv::DescriptorExtractor> extractor;
    detector = cv::FeatureDetector::create("ORB");  
    extractor = cv::DescriptorExtractor::create("ORB");
    detector->detect(image, keypoints);
    extractor->compute(image, keypoints, descriptors);
    string outfile = orbdir+imagename+".xml";
    orb_write(descriptors, outfile);
}

// read orb from files
cv::Mat orb_read(string input_file) {
    cv::Mat descriptors; 
    cv::FileStorage fs(input_file, cv::FileStorage::READ);
    fs["descriptor"] >> descriptors;
    fs.release();
    return descriptors;
}

// check whether the string has the suffix
bool has_suffix(const std::string &str, const std::string &suffix) { 
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

//ratio test nearest/second nearest < ratio
vector<cv::DMatch> ratio_test(vector< vector<cv::DMatch> > matches12, double ratio) {
    vector<cv::DMatch> good_matches;         
    for(int i = 0; i < matches12.size(); i++){  
        if(matches12[i][0].distance < ratio * matches12[i][1].distance)
            good_matches.push_back(matches12[i][0]);
    }
    return good_matches;                  
}

// symmetric Test
vector<cv::DMatch> symmetric_test(std::vector<cv::DMatch> good_matches1, std::vector<cv::DMatch> good_matches2) {
    std::vector<cv::DMatch> better_matches;
    for(int i=0; i<good_matches1.size(); i++){
        for(int j=0; j<good_matches2.size(); j++){
            if(good_matches1[i].queryIdx == good_matches2[j].trainIdx && good_matches2[j].queryIdx == good_matches1[i].trainIdx){
                better_matches.push_back(cv::DMatch(good_matches1[i].queryIdx, good_matches1[i].trainIdx, good_matches1[i].distance));
                break;
            }
        }
    }
    return better_matches;
}

//search orblib to find the max score of image (orbfile has the ".xml")
double ImageSearch(string queryimageorbdir, string queryimageorbname) {
    string orbdir = ORBLIB_DIR;
    string orbfile;
    string queryimageorbfile = queryimageorbdir+queryimageorbname;
    vector <cv::KeyPoint> keypoints1, keypoints2;
    cv::Mat descriptors1, descriptors2;

    vector< vector<cv::DMatch> > matches12, matches21;
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("BruteForce-Hamming");

    descriptors1 = orb_read(queryimageorbfile);

    vector <string> result;
    vector <MatchScore> match_scores;

    priority_queue<MatchScore, std::vector<MatchScore>, ScoreComp> q;
    while (!q.empty()) q.pop();

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(orbdir.c_str())) != NULL) {
        while((ent = readdir(dir)) != NULL) {
            string orbname = (string)(ent->d_name);
            if (has_suffix(orbname, "xml")) {
                orbfile = orbdir + orbname;
                descriptors2 = orb_read(orbfile);

                if(descriptors1.rows != descriptors2.rows)  {
                    cout << "Warning: File rows can't match: "<<"("<<queryimageorbfile<<", "<<orbfile<<")"<<endl;
                    continue;
                }

                if(descriptors1.cols != descriptors2.cols)  {
                    cout << "Warning: File cols can't match: "<<"("<<queryimageorbfile<<", "<<orbfile<<")"<<endl;
                    continue;
                 }
                 if(descriptors1.type() != descriptors2.type() ) {
                    cout << "Warning: File type can't match: "<<"("<<queryimageorbfile<<", "<<orbfile<<")"<<endl;
                    continue;
                 }
                matcher->knnMatch( descriptors1, descriptors2, matches12, 2 );
                matcher->knnMatch( descriptors2, descriptors1, matches21, 2 );

                std::vector<cv::DMatch> good_matches1, good_matches2;
                good_matches1 = ratio_test(matches12, RATIO);
                good_matches2 = ratio_test(matches21, RATIO);

                std::vector<cv::DMatch> better_matches;
                better_matches = symmetric_test(good_matches1, good_matches2);
                
                double jaccard = 1.0 * better_matches.size() / (descriptors1.rows + descriptors2.rows - better_matches.size());
                MatchScore ms;
                ms.query_image_name = queryimageorbname;
                ms.train_image_name = orbname;
                ms.query_image_keypoints_size = descriptors1.rows;
                ms.train_image_keypoints_size = descriptors2.rows;
                ms.good_matches_size =  better_matches.size();
                ms.jaccard = jaccard;
               
                q.push(ms);
            }
        }
    }
    else {
        cout << "Oops, cannot open the directory!\n";
        exit(-1);
    }
    //return the maximum score of (queryfile and orblibfile);

    if (q.empty()) {
        return 0;
    }
    return q.top().jaccard;
}

//get the score between images
double GetScoreBtwImages(string srcOrbDir, string srcOrbfilename, string desOrbDir, string desOrbfilename) {
    vector <cv::KeyPoint> keypoints1, keypoints2;
    cv::Mat descriptors1, descriptors2;

    vector< vector<cv::DMatch> > matches12, matches21;
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("BruteForce-Hamming");

    string srcOrbfile = srcOrbDir + srcOrbfilename;
    string desOrbfile = desOrbDir + desOrbfilename;
    descriptors1 = orb_read(srcOrbfile);
    descriptors2 = orb_read(desOrbfile);
    if(descriptors1.rows != descriptors2.rows)  {
        cout << "Warning: File rows can't match: "<<"("<<srcOrbfile<<", "<<desOrbfile<<")"<<endl;
        return 0;
    }

    if(descriptors1.cols != descriptors2.cols)  {
        cout << "Warning: File cols can't match: "<<"("<<srcOrbfile<<", "<<desOrbfile<<")"<<endl;
        return 0;
     }
     if(descriptors1.type() != descriptors2.type() ) {
        cout << "Warning: File type can't match: "<<"("<<srcOrbfile<<", "<<desOrbfile<<")"<<endl;
        return 0;
     }
    matcher->knnMatch( descriptors1, descriptors2, matches12, 2 );
    matcher->knnMatch( descriptors2, descriptors1, matches21, 2 );

    std::vector<cv::DMatch> good_matches1, good_matches2;
    good_matches1 = ratio_test(matches12, RATIO);
    good_matches2 = ratio_test(matches21, RATIO);

    std::vector<cv::DMatch> better_matches;
    better_matches = symmetric_test(good_matches1, good_matches2);
    
    double jaccard = 1.0 * better_matches.size() / (descriptors1.rows + descriptors2.rows - better_matches.size());
    return jaccard;
} 


