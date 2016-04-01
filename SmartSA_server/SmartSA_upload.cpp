/*
 * the implementation for the functions in SmartSA_upload.h
 * author: Jie Wu(courageJ)
*/
#include "basic_header.h"
#include "direct_upload.h"
#include "SmartSA_upload.h"
#include "orb.h"
#include <iostream>
#include <string>
#include <cstdio>
#include <map>
#include <stack>
#include <errno.h>
#include <iterator>  
#include <dirent.h>
#include <fstream>

using namespace std;


const int MAXSIZE = 500;
map <string, int> Mnameid;
map <int, string> Midname;
vector <string> uploadORB, subgroup;
double battery;
int orbamount;
double weight[MAXSIZE][MAXSIZE];

vector <int> Edge[MAXSIZE];
int dfn[MAXSIZE], low[MAXSIZE], in[MAXSIZE], belong[MAXSIZE];
bool visSCC[MAXSIZE];
int idx;
stack <int> st;
int imagesUploadAmount;

// generate the strong connected  components(scc)
void tarjan(int u){
    dfn[u] = low[u] = ++idx;
    st.push(u);
    in[u] = true;
    for(int i = 0; i < Edge[u].size(); i++){
        int v = Edge[u][i];
        if(!dfn[v]){
            tarjan(v);
            low[u] = min(low[u], low[v]);
        }else
            if(in[v])
            low[u] = min(low[u], dfn[v]);
    }
    int j;
    if(dfn[u]==low[u]){
        imagesUploadAmount++;
        do{
            j = st.top();
            belong[j] = imagesUploadAmount;
            st.pop();
            in[j] = false;
        }while(u != j);
    }
}

//SmartSA Init
void SmartSAinit() {
    battery = 0;
    Midname.clear();
    Mnameid.clear();
    uploadORB.clear();  
    subgroup.clear();
    orbamount = 0;
    memset(weight, 0, sizeof(weight));
    for (int i = 0; i < MAXSIZE; i++) {
        Edge[i].clear();
    }
}

//save orb in temp folder
void WriteOrbtoFile(int connfd) {
    //save battery infomation
    char buf[MAXLEN];
    memset(buf, '\0', sizeof(buf));
    read(connfd, buf, sizeof(int));
    battery = (*(int *)buf) * 0.01;
    cout << "battery = "<<battery <<endl;

    memset(buf, '\0', sizeof(buf));
    read(connfd, buf, sizeof(int));
    orbamount = *(int *)buf;
    cout << "upload orb amount  = " << orbamount << endl;

    for (int id = 0; id < orbamount; id++) {
        int orbsize = 0;
        memset(buf, '\0', sizeof(buf));
        read(connfd, buf, sizeof(int));
        orbsize = *(int *)buf;

        unsigned char *content;
        content = (unsigned char *)malloc(orbsize * sizeof(unsigned char));
        char chid[256] = "";
        sprintf (chid, "%d", id);
        string orbfilename = chid;

        FILE *fp;
        
        string orbfiletype = ".xml";
        orbfilename += orbfiletype;
        uploadORB.push_back(orbfilename);
        Midname[id] = orbfilename;
        Mnameid[orbfilename] = id;
        fp = fopen((TMP_ORB_DIR + orbfilename).c_str(), "w+");
        // ensure TMP_ORBcout <<"id = "<<id<<endl;_DIR exists;

        //add ".xml" info
        char chxmlinfo[1000];
        string strxmlinfo = "<?xml version=\"1.0\"\?>\n<opencv_storage>\n<descriptor type_id=\"opencv-matrix\">\n  <rows>";
        char *chrows;
        chrows = (char *)malloc(sizeof(int));
        sprintf (chrows, "%d", orbsize / 32);
        strxmlinfo += chrows;
        strxmlinfo +="</rows>\n  <cols>32</cols>\n  <dt>u</dt>\n  <data>\n";

        memset(chxmlinfo, '\0', sizeof(chxmlinfo));
        sprintf (chxmlinfo, "%s", strxmlinfo.c_str());
        fwrite(chxmlinfo, sizeof(char), strxmlinfo.length(), fp);

        const int ORB_SIZE = 1024;
        int nwritten = 0, n;
        while(nwritten < orbsize) {
            if (orbsize - nwritten < ORB_SIZE) {
                n = Readn(connfd, content, orbsize - nwritten);
            }
            else {
                n = Readn(connfd, content, ORB_SIZE);
            }
            nwritten += n;
            for (int i = 0; i < n; i++) {
                int num = (int)*(content + i);
                if ((nwritten - n + i + 1) % 32) {
                    fprintf(fp, "%d ", num);
                }
                else {
                    fprintf (fp, "%d\n", num);
                }
            }
        }

        //add ".xml" info
        strxmlinfo = "</data></descriptor>\n</opencv_storage>";
        sprintf (chxmlinfo, "%s", strxmlinfo.c_str());
        fwrite(chxmlinfo, sizeof(char), strxmlinfo.length(), fp);

        free(content);
        fclose(fp);
    }
    //cout <<"WriteOrbtoFile done!" << endl;
}

//count the score based on battery, and select file over the score
void GenSubGroup() {
    double Tscore = 0.013 + kRed * battery;
    for (int upid = 0; upid < uploadORB.size(); upid++) {
        string orbfilename = uploadORB[upid];
        double score = ImageSearch(TMP_ORB_DIR, orbfilename);
        if (score < 0 || score > 1) {
            score = 0;
        }
        if (score <= Tscore) {
            subgroup.push_back(orbfilename);
        }
    }
}

// calculate redundancy w(i, j) between images
void CalcWeight() {
    for (int src = 0; src< subgroup.size(); src++) {
            string srcfilename = subgroup[src];
            int srcid = Mnameid[srcfilename];
            weight[srcid][srcid] = 1;
        for (int des = src + 1; des < subgroup.size(); des++) {
            string desfilename = subgroup[des];
            int desid = Mnameid[desfilename];
            weight[srcid][desid] = weight[desid][srcid] = GetScoreBtwImages(TMP_ORB_DIR, srcfilename, TMP_ORB_DIR, desfilename);
        }
    }
}


//calculate number of images should be uploaded
void CalcUploadAmount() {
    double Tscore = 0.013 + kRed * battery;
    for (int src = 0; src < subgroup.size(); src++) {
        for (int des = src + 1; des < subgroup.size(); des++) {
            string srcfilename = subgroup[src];
            string desfilename = subgroup[des];
            int srcid = Mnameid[srcfilename];
            int desid = Mnameid[desfilename];
            if (weight[srcid][desid] >= Tscore || weight[desid][srcid] >= Tscore) {
                Edge[srcid].push_back(desid);
                Edge[desid].push_back(srcid);
            }
        }
    }
    memset(low, 0, sizeof(low));
    memset(dfn, 0, sizeof(dfn));
    memset(in, 0, sizeof(in));
    memset(belong, -1, sizeof(belong));
    idx = 0, imagesUploadAmount = 0;
    for (int i = 0; i < subgroup.size(); i++) {
        int id = Mnameid[subgroup[i]];
        if (!dfn[id]) {
            tarjan(id);
        }
    }
    cout << "amount of images to  be uploaded  = " <<imagesUploadAmount << endl;
}

map <string, int> isSelect;
vector <string> selectedImage;

//submodular subfunction: Coverage
double Coverage() {
    double ret = 0;
    for (int i = 0; i < subgroup.size(); i++) {
        double tmp = 0;
        for (int j = 0; j < selectedImage.size(); j++) {
            int srcid = Mnameid[subgroup[i]];
            int desid = Mnameid[selectedImage[j]];
            tmp = max(tmp, weight[srcid][desid]);
        }
        ret += tmp;
    }
    return ret;
}

//submodular subfunction: diversity
int Diversity() {
    int ret = 0;
    memset(visSCC, 0, sizeof(visSCC));
    for (int i = 0; i < selectedImage.size(); i++) {
        int id = Mnameid[selectedImage[i]];
        int sccid = belong[id];
        if (sccid == -1) {
            puts("Images name error!"); 
            exit(-1);
        }
        if (visSCC[sccid]) continue;
        else {
            visSCC[sccid] = 1;
            ret ++;
        }
    }
    return ret;
}

//submodular function
double Submodular_Func() {
    double ret = 0;
    ret = kCov * Coverage() + kDiv * Diversity();
    return ret;
}

//return id of images to be uploaded based on submodular function
string submodular() {
    selectedImage.clear();
    isSelect.clear();

    if (imagesUploadAmount >= 1) {
        srand(time(NULL));
        int randid = rand() % subgroup.size();
        string v1 = subgroup[randid];
        selectedImage.push_back(v1);
        double submodularScore = Submodular_Func();
        isSelect[v1] = 1;
        for (int i = 1; i < imagesUploadAmount; i++) {
            double preScore = -1;
            string selectStr = "";
            for (int j = 0; j < subgroup.size(); j++) {
                string filename = subgroup[j];
                if (isSelect[filename] == 1) continue;
                selectedImage.push_back(filename);
                double curScore = Submodular_Func();
                if (curScore > preScore) {
                    selectStr = filename;
                    preScore = curScore;
                }
                selectedImage.pop_back();
            }
            if (preScore < submodularScore) {
                imagesUploadAmount = i - 1;
                break;
            }
            else {
                selectedImage.push_back(selectStr);
                isSelect[selectStr] = 1;
                submodularScore = preScore;
            }
        }
    }
    
    bool vis[MAXSIZE];
    memset(vis, 0, sizeof(vis));
    for (int i = 0; i < selectedImage.size(); i ++) {
        string selectStr = selectedImage[i];
        vis[Mnameid[selectStr]] = 1;
    }
    string ret = "";
    for (int i = 0; i < orbamount; i++) {
        if (vis[i]) {
            ret +="1";
        }
        else {
            ret += "0";
        }
    }
    cout << ret << endl;
    return ret;
}

//get the files id
string GetUploadFile() {
    GenSubGroup();
    CalcWeight();
    CalcUploadAmount();
    return submodular();
}








