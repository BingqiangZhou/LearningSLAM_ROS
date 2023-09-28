#include <iostream>
#include <fstream>

#include <ros/ros.h>
#include <exception>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

#pragma region 函数声明

Mat GetShowImgAdjust(Mat matImg, double &dShowImgRatio,  int nFixWidth=1200, int nFixHeight=800);

vector<Point> GetRectPoints(Mat matImg, double nDownSizeFactor=0.1);

int GetRandNumber(int minValue=0, int maxValue=50);

Mat ColorMat2Gray(Mat matImg);


int Simulate1(){
  ros::NodeHandle nh; // 需要注意局部与全局的区别  https://blog.csdn.net/qq_39779233/article/details/108411778
   // bool bLoadData = false;
  bool bLoadData = true;
  
#pragma region 获取输入图像路径
  std::string strInputImgPath;
  std::string strDataPath;
  ifstream ifs;
  if(nh.getParam("simulate/strDataPath", strDataPath)){
    ifs.open(strDataPath);
    if(ifs.is_open()){
      // bLoadData = true;
      ifs.close();
      ROS_INFO("input data path: %s", strDataPath.c_str());
    }
  }
  if(nh.getParam("simulate/strInputImagePath", strInputImgPath)){
    // ROS_INFO("input image path: %s", strInputImgPath.c_str());
  }
  else{
    ROS_ERROR("didn't find parameter `strInputImagePath`");
    return -1;
  }
  bool bShowRestructMap;
  nh.param<bool>("simulate/bShowRestructMap", bShowRestructMap, false);
#pragma endregion
 
#pragma region 读取图像 
  Mat matImg = cv::imread(strInputImgPath.c_str());
  if(matImg.empty()){
    printf("img is empty\n");
    return -2;
  }
#pragma endregion

  double dShowImgRatio;
  double nDownSizeFactor=0.2;
  Mat matImgAdjust = GetShowImgAdjust(matImg, dShowImgRatio);
  Mat matShowImg = matImgAdjust.clone();
  //printf("pt num %d\n", matShowImg.cols);
  vector<Point> vecPts = GetRectPoints(matShowImg);
  //printf("pt num %d\n", vecPts.size());
  vector<Point> vecPtsForImu(vecPts.size()); // 惯性
  vector<Point> vecPtsForImuView(vecPts.size()); // 视觉+惯性
  vector<Point> vecPtsForLoopClosure(vecPts.size()); // 回环检测
  
  printf("open data, %d\n", bLoadData);
  if (bLoadData){
    ifs.open(strDataPath, ios::in | ios::binary);
    if (!ifs.is_open()){
      return 0;
    }
    int nPtNum = 0;
    ifs >> nPtNum;
    printf("nPtNum %d\n", nPtNum);
    vecPts.resize(nPtNum);
    for (int j = 0; j < nPtNum; j++){
      ifs >>  vecPts[j].x >> vecPts[j].y;
      // printf("%d, %d\n", vecPts[j].x, vecPts[j].y);
    }
    vecPtsForImu.resize(nPtNum);
    for (int j = 0; j < nPtNum; j++){
      ifs >>  vecPtsForImu[j].x >>  vecPtsForImu[j].y;
      // printf("%d, %d\n", vecPtsForImu[j].x, vecPtsForImu[j].y);

    }
    vecPtsForImuView.resize(nPtNum);
    for (int j = 0; j < nPtNum; j++){
      ifs >>  vecPtsForImuView[j].x >>  vecPtsForImuView[j].y;
      // printf("%d, %d\n", vecPtsForImuView[j].x, vecPtsForImuView[j].y);

    }
    vecPtsForLoopClosure.resize(nPtNum);
    for (int j = 0; j < nPtNum; j++){
      ifs >>  vecPtsForLoopClosure[j].x >>  vecPtsForLoopClosure[j].y;
      // printf("%d, %d\n", vecPtsForLoopClosure[j].x, vecPtsForLoopClosure[j].y);

    }
    ifs.close();
  }


  Mat matShowRestructMap;
  if(bShowRestructMap){
    matShowRestructMap = Mat(matShowImg.size(), CV_8UC3, Scalar(255, 255, 255));
  }

  double nRandRangeForImu = 0.2;
  double dRandSumXForImu = 0, dRandSumYForImu = 0;
  double nRandRangeForImuView = 0.4;
  double dRandSumXForImuView = 0, dRandSumYForImuView = 0;
  double nRandRangeForloopClosure = 0.2;
  double dRandSumXForloopClosure = 0, dRandSumYForloopClosure = 0;
  int nRectHeightWidth = MAX(matImg.cols, matImg.rows) * nDownSizeFactor / 4 * dShowImgRatio;
  int nRandHeight = 0, nRandWidth = 0;
  for (int i = 0; i < vecPts.size(); i++){

    // 计算加上随机性的高宽
    int nRandWidth =  nRectHeightWidth + GetRandNumber(0, MAX(nRectHeightWidth * 0.5 * 5, 5)) / 5;
    int nRandHeight = nRectHeightWidth + GetRandNumber(0, MAX(nRectHeightWidth * 0.5 * 5, 5)) / 5;
    // printf("%d, %d; %d, %d\n", vecPts[i].x, vecPts[i].y,  nRandWidth, nRandWidth);
    Rect rectLoadImg(vecPts[i].x - nRandWidth / 2, vecPts[i].y - nRandHeight / 2, nRandWidth, nRandHeight);
    //printf("%d, %d, %d, %d\n", rectLoadImg.x, rectLoadImg.y, rectLoadImg.width, rectLoadImg.height);
   
    // 提取附近图像
    Mat matLoadImg = matImgAdjust(rectLoadImg).clone();
    // 将rgb图转为灰度图，再合并为3通道的图，代替大图位置的彩色图
    Mat matMergeGray = ColorMat2Gray(matLoadImg);
    matMergeGray.copyTo(matShowImg(rectLoadImg));

    if (!bLoadData){
      // 由上一点和这一个点，判断随机x还是随机y
      int nLastPtIndex = (i - 1 + vecPts.size()) % vecPts.size();
      bool bRandSign = 1;
      if(vecPts[i].x - vecPts[nLastPtIndex].x != 0){
        // printf("%d, %d; %d, %d; 1 %d\n", i, nLastPtIndex, vecPts[i].x, vecPts[nLastPtIndex].x, bRandSign);
        dRandSumYForImu += bRandSign * (GetRandNumber(0, nRandRangeForImu * 100) / 100.0);
        dRandSumYForImuView  += bRandSign * (GetRandNumber(0, nRandRangeForImuView * 100) / 100.0 - nRandRangeForImuView/2);
        dRandSumYForloopClosure += (GetRandNumber(0, nRandRangeForloopClosure * 100) / 100.0 - nRandRangeForloopClosure/2);
      }
      else{
        // printf("%d, %d; %d, %d; 1 %d\n", i, nLastPtIndex, vecPts[i].x, vecPts[nLastPtIndex].x, bRandSign);
        dRandSumXForImu += bRandSign * (GetRandNumber(0, nRandRangeForImu * 100) / 100.0);
        dRandSumXForImuView  += bRandSign * (GetRandNumber(0, nRandRangeForImuView * 100) / 100.0 - nRandRangeForImuView/2);
        dRandSumXForloopClosure += (GetRandNumber(0, nRandRangeForloopClosure * 100) / 100.0 - nRandRangeForloopClosure/2);
      }
      vecPtsForImu[i] = vecPts[i] + Point(dRandSumXForImu, dRandSumYForImu);
      vecPtsForImuView[i] = vecPts[i] + Point(dRandSumXForImuView, dRandSumYForImuView);
      vecPtsForLoopClosure[i] = vecPts[i] + Point(dRandSumXForloopClosure, dRandSumYForloopClosure);
    }

    Mat matShowImgTemp = matShowImg.clone();
    // 画出航线
    for (int j = 0; j <= i; j++){
      circle(matShowImgTemp, vecPtsForImu[j], 1, Scalar(0, 255, 255)); // 显示航线
      circle(matShowImgTemp, vecPtsForImuView[j], 1, Scalar(0, 0, 255)); // 显示航线
      circle(matShowImgTemp, vecPts[j], 1, Scalar(255, 0, 0)); // 显示GPS航线
    }

    if(bShowRestructMap){
      matMergeGray.copyTo(matShowRestructMap(rectLoadImg));
      imshow("RestructMap", matShowRestructMap);
    }

    //显示中间结果
    // rectangle(matShowImg, rectLoadImg, Scalar(0, 255, 0));
    imshow("img", matShowImgTemp);
    // imshow("gray", matMergeGray);
    char c = waitKey(50);
    if(c == 'c' || c == 'C'){
      destroyAllWindows();
      return 0;
    }

  }
  printf("loop closure...\n");
  sleep(0.2);
  for (int j = 0; j < vecPts.size(); j++){
    circle(matShowImg, vecPtsForImu[j], 1, Scalar(0, 255, 255)); // 显示航线
    circle(matShowImg, vecPts[j], 1, Scalar(255, 0, 0)); // 显示GPS航线
    circle(matShowImg, vecPtsForLoopClosure[j], 1, Scalar(0, 255, 0)); // 显示GPS航线
  }
  line(matShowImg, vecPtsForLoopClosure[vecPts.size() - 1], vecPtsForLoopClosure[0], Scalar(0, 255, 0), 2);

  // 保存数据
  if (!bLoadData){
    ofstream ofs(strDataPath, ios::out | ios::binary);
    if (!ofs.is_open()){
      destroyAllWindows();
      return 0;
    }
    ofs << int(vecPts.size()) << endl;
    for (int j = 0; j < vecPts.size(); j++){
      ofs <<  vecPts[j].x << " " <<  vecPts[j].y << endl;
    }
    for (int j = 0; j < vecPts.size(); j++){
      ofs <<  vecPtsForImu[j].x << " " <<  vecPtsForImu[j].y << endl;
    }
    for (int j = 0; j < vecPts.size(); j++){
      ofs <<  vecPtsForImuView[j].x  << " " <<  vecPtsForImuView[j].y << endl;
    }
    for (int j = 0; j < vecPts.size(); j++){
      ofs <<  vecPtsForLoopClosure[j].x << " " <<  vecPtsForLoopClosure[j].y << endl;
    }
    ofs.close();
  }
  printf("data saved.\n");
  imshow("img", matShowImg);
  waitKey(0);
  destroyAllWindows();
}

int Simulate2(){
  ros::NodeHandle nh; // 需要注意局部与全局的区别  https://blog.csdn.net/qq_39779233/article/details/108411778
  //  bool bLoadData = false;
  bool bLoadData = true;
  
#pragma region 获取输入图像路径
  std::string strInputImgPath;
  std::string strDataPath;
  ifstream ifs;
  if(nh.getParam("simulate/strDataPathForMutli", strDataPath)){
    ifs.open(strDataPath);
    if(ifs.is_open()){
      // bLoadData = true;
      ifs.close();
      ROS_INFO("input data path: %s", strDataPath.c_str());
    }
  }
  if(nh.getParam("simulate/strInputImagePath", strInputImgPath)){
    // ROS_INFO("input image path: %s", strInputImgPath.c_str());
  }
  else{
    ROS_ERROR("didn't find parameter `strInputImagePath`");
    return -1;
  }
  // bool bShowRestructMap;
  // nh.param<bool>("simulate/bShowRestructMap", bShowRestructMap, false);
#pragma endregion
 
#pragma region 读取图像 
  Mat matImg = cv::imread(strInputImgPath.c_str());
  if(matImg.empty()){
    printf("img is empty\n");
    return -2;
  }
#pragma endregion

  double dShowImgRatio;
  double nDownSizeFactor=0.2;
  Mat matImgAdjust = GetShowImgAdjust(matImg, dShowImgRatio);
  Mat matShowImg = matImgAdjust.clone();
  //printf("pt num %d\n", matShowImg.cols);
  vector<Point> vecPts = GetRectPoints(matShowImg);  
  //printf("pt num %d\n", vecPts.size());
  vector<Point> vecPtsForImu(vecPts.size()); // 惯性
  vector<Point> vecPtsForImuView(vecPts.size()); // 视觉+惯性
  vector<Point> vecPtsForLoopClosure(vecPts.size()); // 回环检测
  
  printf("open data, %d\n", bLoadData);
  if (bLoadData){
    ifs.open(strDataPath, ios::in | ios::binary);
    if (!ifs.is_open()){
      return 0;
    }
    int nPtNum = 0;
    ifs >> nPtNum;
    printf("nPtNum %d\n", nPtNum);
    vecPts.resize(nPtNum);
    for (int j = 0; j < nPtNum; j++){
      ifs >>  vecPts[j].x >> vecPts[j].y;
      // printf("%d, %d\n", vecPts[j].x, vecPts[j].y);
    }
    vecPtsForImu.resize(nPtNum);
    for (int j = 0; j < nPtNum; j++){
      ifs >>  vecPtsForImu[j].x >>  vecPtsForImu[j].y;
      // printf("%d, %d\n", vecPtsForImu[j].x, vecPtsForImu[j].y);

    }
    vecPtsForImuView.resize(nPtNum);
    for (int j = 0; j < nPtNum; j++){
      ifs >>  vecPtsForImuView[j].x >>  vecPtsForImuView[j].y;
      // printf("%d, %d\n", vecPtsForImuView[j].x, vecPtsForImuView[j].y);

    }
    vecPtsForLoopClosure.resize(nPtNum);
    for (int j = 0; j < nPtNum; j++){
      ifs >>  vecPtsForLoopClosure[j].x >>  vecPtsForLoopClosure[j].y;
      // printf("%d, %d\n", vecPtsForLoopClosure[j].x, vecPtsForLoopClosure[j].y);

    }
    ifs.close();
  }

  double nRandRangeForImu = 0.2;
  double dRandSumXForImu = 0, dRandSumYForImu = 0;
  double nRandRangeForImuView = 0.4;
  double dRandSumXForImuView = 0, dRandSumYForImuView = 0;
  double nRandRangeForloopClosure = 0.2;
  double dRandSumXForloopClosure = 0, dRandSumYForloopClosure = 0;
  int nRectHeightWidth = MAX(matImg.cols, matImg.rows) * nDownSizeFactor / 4 * dShowImgRatio;
  int nRandHeight = 0, nRandWidth = 0;
  for (int i = 0; i < vecPts.size() / 2; i++){

    int nIndex1 = i;
    int nIndex2 = vecPts.size() - i - 1;
    Point pt1 = vecPts[i];
    Point pt2 = vecPts[vecPts.size() - i - 1];

    // 计算加上随机性的高宽
    int nRandWidth =  nRectHeightWidth + GetRandNumber(0, MAX(nRectHeightWidth * 0.5 * 5, 5)) / 5;
    int nRandHeight = nRectHeightWidth + GetRandNumber(0, MAX(nRectHeightWidth * 0.5 * 5, 5)) / 5;
    // printf("%d, %d; %d, %d\n", vecPts[i].x, vecPts[i].y,  nRandWidth, nRandWidth);
    Rect rectLoadImg1(pt1.x - nRandWidth / 2, pt1.y - nRandHeight / 2, nRandWidth, nRandHeight);
    //printf("%d, %d, %d, %d\n", rectLoadImg.x, rectLoadImg.y, rectLoadImg.width, rectLoadImg.height);
   
    // 提取附近图像
    Mat matLoadImg1 = matImgAdjust(rectLoadImg1).clone();
    // 将rgb图转为灰度图，再合并为3通道的图，代替大图位置的彩色图
    Mat matMergeGray1 = ColorMat2Gray(matLoadImg1);
    matMergeGray1.copyTo(matShowImg(rectLoadImg1));

    // 计算加上随机性的高宽
    nRandWidth =  nRectHeightWidth + GetRandNumber(0, MAX(nRectHeightWidth * 0.5 * 5, 5)) / 5;
    nRandHeight = nRectHeightWidth + GetRandNumber(0, MAX(nRectHeightWidth * 0.5 * 5, 5)) / 5;
    // printf("%d, %d; %d, %d\n", vecPts[i].x, vecPts[i].y,  nRandWidth, nRandWidth);
    Rect rectLoadImg2(pt2.x - nRandWidth / 2, pt2.y - nRandHeight / 2, nRandWidth, nRandHeight);
    //printf("%d, %d, %d, %d\n", rectLoadImg.x, rectLoadImg.y, rectLoadImg.width, rectLoadImg.height);
   
    // 提取附近图像
    Mat matLoadImg2 = matImgAdjust(rectLoadImg2).clone();
    // 将rgb图转为灰度图，再合并为3通道的图，代替大图位置的彩色图
    Mat matMergeGray2 = ColorMat2Gray(matLoadImg2);
    matMergeGray2.copyTo(matShowImg(rectLoadImg2));

    if (!bLoadData){
      // 由上一点和这一个点，判断随机x还是随机y
      int nLastPtIndex = (nIndex1 - 1 + vecPts.size()) % vecPts.size();
      if(vecPts[nIndex1].x - vecPts[nLastPtIndex].x != 0){
        // printf("%d, %d; %d, %d; 1 %d\n", i, nLastPtIndex, vecPts[i].x, vecPts[nLastPtIndex].x, bRandSign);
        dRandSumYForImu += (GetRandNumber(0, nRandRangeForImu * 100) / 100.0);
        dRandSumYForImuView  += (GetRandNumber(0, nRandRangeForImuView * 100) / 100.0 - nRandRangeForImuView/2);
        dRandSumYForloopClosure += (GetRandNumber(0, nRandRangeForloopClosure * 100) / 100.0 - nRandRangeForloopClosure/2);
      }
      else{
        // printf("%d, %d; %d, %d; 1 %d\n", i, nLastPtIndex, vecPts[i].x, vecPts[nLastPtIndex].x, bRandSign);
        dRandSumXForImu += (GetRandNumber(0, nRandRangeForImu * 100) / 100.0);
        dRandSumXForImuView  += (GetRandNumber(0, nRandRangeForImuView * 100) / 100.0 - nRandRangeForImuView/2);
        dRandSumXForloopClosure += (GetRandNumber(0, nRandRangeForloopClosure * 100) / 100.0 - nRandRangeForloopClosure/2);
      }
      vecPtsForImu[nIndex1] = vecPts[nIndex1] + Point(dRandSumXForImu, dRandSumYForImu);
      vecPtsForImuView[nIndex1] = vecPts[nIndex1] + Point(dRandSumXForImuView, dRandSumYForImuView);
      vecPtsForLoopClosure[nIndex1] = vecPts[nIndex1] + Point(dRandSumXForloopClosure, dRandSumYForloopClosure);

      // 由上一点和这一个点，判断随机x还是随机y
      int nLastPtIndexEnd = (nIndex2 + 1 + vecPts.size()) % vecPts.size();
      if(vecPts[nIndex2].x - vecPts[nLastPtIndex].x != 0){
        // printf("%d, %d; %d, %d; 1 %d\n", i, nLastPtIndex, vecPts[i].x, vecPts[nLastPtIndex].x, bRandSign);
        dRandSumYForImu += (GetRandNumber(0, nRandRangeForImu * 100) / 100.0);
        dRandSumYForImuView  += (GetRandNumber(0, nRandRangeForImuView * 100) / 100.0 - nRandRangeForImuView/2);
        dRandSumYForloopClosure += (GetRandNumber(0, nRandRangeForloopClosure * 100) / 100.0 - nRandRangeForloopClosure/2);
      }
      else{
        // printf("%d, %d; %d, %d; 1 %d\n", i, nLastPtIndex, vecPts[i].x, vecPts[nLastPtIndex].x, bRandSign);
        dRandSumXForImu += (GetRandNumber(0, nRandRangeForImu * 100) / 100.0);
        dRandSumXForImuView  += (GetRandNumber(0, nRandRangeForImuView * 100) / 100.0 - nRandRangeForImuView/2);
        dRandSumXForloopClosure += (GetRandNumber(0, nRandRangeForloopClosure * 100) / 100.0 - nRandRangeForloopClosure/2);
      }
      vecPtsForImu[nIndex2] = vecPts[nIndex2] + Point(dRandSumXForImu, dRandSumYForImu);
      vecPtsForImuView[nIndex2] = vecPts[nIndex2] + Point(dRandSumXForImuView, dRandSumYForImuView);
      vecPtsForLoopClosure[nIndex2] = vecPts[nIndex2] + Point(dRandSumXForloopClosure, dRandSumYForloopClosure);
    }

    Mat matShowImgTemp = matShowImg.clone();
    // 画出航线
    for (int j = 0; j <= i; j++){
      circle(matShowImgTemp, vecPtsForImu[j], 1, Scalar(0, 255, 255)); // 显示航线
      circle(matShowImgTemp, vecPtsForImuView[j], 1, Scalar(0, 0, 255)); // 显示航线
      circle(matShowImgTemp, vecPts[j], 1, Scalar(255, 0, 0)); // 显示GPS航线
    }
    // 画出航线
    for (int j = vecPts.size()-1; j >= nIndex2; j--){
      circle(matShowImgTemp, vecPtsForImu[j], 1, Scalar(0, 255, 255)); // 显示航线
      circle(matShowImgTemp, vecPtsForImuView[j], 1, Scalar(0, 0, 255)); // 显示航线
      circle(matShowImgTemp, vecPts[j], 1, Scalar(255, 0, 0)); // 显示GPS航线
    }

    //显示中间结果
    // rectangle(matShowImg, rectLoadImg, Scalar(0, 255, 0));
    imshow("img", matShowImgTemp);
    // imshow("gray", matMergeGray);
    char c = waitKey(50);
    if(c == 'c' || c == 'C'){
      destroyAllWindows();
      return 0;
    }

  }
  printf("loop closure...\n");
  sleep(0.2);
  for (int j = 0; j < vecPts.size(); j++){
    circle(matShowImg, vecPtsForImu[j], 1, Scalar(0, 255, 255)); // 显示航线
    circle(matShowImg, vecPts[j], 1, Scalar(255, 0, 0)); // 显示GPS航线
    circle(matShowImg, vecPtsForLoopClosure[j], 1, Scalar(0, 255, 0)); // 显示GPS航线
  }
  line(matShowImg, vecPtsForLoopClosure[vecPts.size() - 1], vecPtsForLoopClosure[0], Scalar(0, 255, 0), 2);

  // 保存数据
  if (!bLoadData){
    ofstream ofs(strDataPath, ios::out | ios::binary);
    if (!ofs.is_open()){
      destroyAllWindows();
      return 0;
    }
    ofs << int(vecPts.size()) << endl;
    for (int j = 0; j < vecPts.size(); j++){
      ofs <<  vecPts[j].x << " " <<  vecPts[j].y << endl;
    }
    for (int j = 0; j < vecPts.size(); j++){
      ofs <<  vecPtsForImu[j].x << " " <<  vecPtsForImu[j].y << endl;
    }
    for (int j = 0; j < vecPts.size(); j++){
      ofs <<  vecPtsForImuView[j].x  << " " <<  vecPtsForImuView[j].y << endl;
    }
    for (int j = 0; j < vecPts.size(); j++){
      ofs <<  vecPtsForLoopClosure[j].x << " " <<  vecPtsForLoopClosure[j].y << endl;
    }
    ofs.close();
  }
  printf("data saved.\n");
  imshow("img", matShowImg);
  waitKey(0);
  destroyAllWindows();
}

#pragma endregion



#pragma region 函数定义
Mat GetShowImgAdjust(Mat matImg, double &dShowImgRatio, int nFixWidth, int nFixHeight){
  // 计算合适的显示大小
  dShowImgRatio = 1.0;
  int w = matImg.cols, h = matImg.rows;
  printf("source image size(w, h): (%d, %d)\n", w, h);
  if(w > h){
      //c 宽大于高，按宽为1200计算比例
      dShowImgRatio = nFixWidth * 1.0 / w;
  }
  else{
      // 高大于宽，按高为800计算比例
      dShowImgRatio = nFixHeight * 1.0 / h;
  }
  printf("show image ratio: %f\n", dShowImgRatio);
  Mat matShowImg;
  cv::resize(matImg, matShowImg, cv::Size(0, 0), dShowImgRatio, dShowImgRatio);
  printf("show image size(w, h): (%d, %d)\n", matShowImg.cols, matShowImg.rows);
  return matShowImg;
}

vector<Point> GetRectPoints(Mat matImg, double nDownSizeFactor){
  if (nDownSizeFactor <= 0 || nDownSizeFactor >= 0.5){
    printf("nDownSizeFactor must be in range (0, 0.5)");
    throw "nDownSizeFactor must be in range (0, 0.5)";
    return vector<Point>();
  }
  
  int nDownSizeWidth = int(matImg.cols * nDownSizeFactor);
  int nDownSizeHeight = int(matImg.rows * nDownSizeFactor);
  Rect rectDownSize = Rect(nDownSizeWidth, nDownSizeHeight, matImg.cols - nDownSizeWidth * 2, matImg.rows - nDownSizeHeight * 2);

  // printf("%d, %d, %d, %d\n", rectDownSize.x, rectDownSize.y, rectDownSize.width, rectDownSize.height);

  vector<Point> vecPtList;
  vecPtList.resize(max(rectDownSize.width, rectDownSize.height) * 4);
  int nIndex = 0;
  // 左上到左下
  for (int i = rectDownSize.y; i <= rectDownSize.y + rectDownSize.height; i++, nIndex++){
    vecPtList[nIndex] = Point(rectDownSize.x, i); 
  }
  // 左下到右下
  for (int i = rectDownSize.x; i <= rectDownSize.x + rectDownSize.width; i++, nIndex++){
    vecPtList[nIndex] = Point(i, rectDownSize.y + rectDownSize.height); 
  }
  // 右下到右上
  for (int i = rectDownSize.y + rectDownSize.height; i >= rectDownSize.y; i--, nIndex++){
    vecPtList[nIndex] = Point(rectDownSize.x + rectDownSize.width, i); 
  }
  // 右上到左上
  for (int i = rectDownSize.x + rectDownSize.width; i >= rectDownSize.x; i--, nIndex++){
    vecPtList[nIndex] = Point(i, rectDownSize.y); 
  }
  vecPtList.resize(nIndex);  

  return vecPtList;
}

int GetRandNumber(int minValue, int maxValue){
  srand(time(nullptr)); // 用当前时间作为种子
  int randomValue = (rand() % int(maxValue - minValue)) + minValue + 1;//范围(min,max]
  return randomValue;
}

Mat ColorMat2Gray(Mat matImg){
  Mat matLoadImgGray;
  cvtColor(matImg, matLoadImgGray, COLOR_BGR2GRAY);
  matLoadImgGray = matLoadImgGray + 50; // 增加50
  vector<Mat> vecMatGray = { matLoadImgGray, matLoadImgGray, matLoadImgGray};
  Mat matMergeGray;
  merge(vecMatGray, matMergeGray);
  return matMergeGray;
}
#pragma endregion


int main(int argc, char **argv)
{

  ros::init(argc, argv, "simulate");

  while (1)
  {
    printf("hello, world!\n");
    printf("--------------------------------\n");
    printf("请选择模拟模式\n");
    printf("1.模拟单个飞行器回环检测\n");
    printf("2.模拟多个飞行器回环检测\n");
    printf("3.退出\n");
    int nInput;
    cin >> nInput;
    switch (nInput)
    {
    case 1:
      printf("开始模拟单个飞行器回环检测，按`c`键可退出\n");
      Simulate1();
      break;
    case 2:
      printf("开始模拟多个飞行器回环检测，按`c`键可退出\n");
      Simulate2();
      break;
    case 3:
      /* code */
      break;
    default:
      printf("请输入正确数字1-3!\n");
      break;
    }
    if (nInput == 3){
      break;
    }
  }


//   ros::Rate rate(100);
//   while (node.isOkay())
//   {
//     ros::spinOnce();
//     node.spin();
//     rate.sleep();
//   }
  ros::shutdown();
  return 0;
}