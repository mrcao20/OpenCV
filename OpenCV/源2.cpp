#include "stdafx.h"
#include <iostream>   
#include <iomanip>  
#include <opencv2\opencv.hpp>
#include "opencv2/core/core.hpp"  
#include "opencv2/objdetect/objdetect.hpp"   
#include "opencv2/features2d/features2d.hpp"  
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/calib3d/calib3d.hpp"  
#include "opencv2/imgproc/imgproc_c.h"  
#include <opencv2\xfeatures2d.hpp>
  
using namespace cv;   
using namespace std;  
using namespace xfeatures2d;
 
//int main()  
//  
//{  
//  
//    /*Mat leftImg=imread("left.jpg");  
//    Mat rightImg=imread("right.jpg");  */
//	Mat leftImg=imread("E:\\screenshots\\tempNULL\\12.jpg");  
//    Mat rightImg=imread("E:\\screenshots\\tempNULL\\13.jpg");  
//  
//    if(leftImg.data==NULL||rightImg.data==NULL)  
//  
//        return 0;  
//  
//   
//  
//    //转化成灰度图  
//  
//    Mat leftGray;  
//  
//    Mat rightGray;  
//  
//    cvtColor(leftImg,leftGray,CV_BGR2GRAY);  
//  
//    cvtColor(rightImg,rightGray,CV_BGR2GRAY);  
//  
//   
//  
//    //获取两幅图像的共同特征点  
//  
//    int minHessian=400;  
//  
//    Ptr<FeatureDetector> detector = SURF::create(minHessian);  
//  
//    vector<KeyPoint> leftKeyPoints,rightKeyPoints;  
//  
//    detector->detect(leftGray,leftKeyPoints);  
//  
//    detector->detect(rightGray,rightKeyPoints);  
//  
//    Ptr<SURF>extractor = SURF::create();
//  
//    Mat leftDescriptor,rightDescriptor;  
//  
//    extractor->compute(leftGray,leftKeyPoints,leftDescriptor);  
//  
//    extractor->compute(rightGray,rightKeyPoints,rightDescriptor);  
//  
//    FlannBasedMatcher matcher;  
//	//BFMatcher matcher;
//  
//    vector<DMatch> matches;  
//  
//    matcher.match(leftDescriptor,rightDescriptor,matches);      
//  
//    int matchCount=leftDescriptor.rows;  
//  
//    if(matchCount>15)  
//  
//    {  
//  
//        matchCount=15;  
//  
//        //sort(matches.begin(),matches.begin()+leftDescriptor.rows,DistanceLessThan);  
//  
//        sort(matches.begin(),matches.begin()+leftDescriptor.rows);  
//  
//    }      
//
//	/*Mat img_matches;
//	  drawMatches( leftImg, leftKeyPoints, rightImg, rightKeyPoints,
//               matches, img_matches, Scalar::all(-1), Scalar::all(-1),
//               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
//	  imshow( "匹配成功", img_matches );
//  waitKey(0);*/
//  
//    vector<Point2f> leftPoints;  
//  
//    vector<Point2f> rightPoints;  
//  
//    for(int i=0; i<matchCount; i++)  
//  
//    {  
//  
//        leftPoints.push_back(leftKeyPoints[matches[i].queryIdx].pt);  
//  
//        rightPoints.push_back(rightKeyPoints[matches[i].trainIdx].pt);  
//  
//    }  
//  
//   
//  
//    //获取左边图像到右边图像的投影映射关系  
//  
//    Mat homo=findHomography(leftPoints,rightPoints);  
//  
//    Mat shftMat=(Mat_<double>(3,3)<<1.0,0,leftImg.cols, 0,1.0,0, 0,0,1.0);  
//  
//   
//  
//    //拼接图像  
//  
//    Mat tiledImg;  
//  
//    warpPerspective(leftImg,tiledImg,shftMat*homo,Size(leftImg.cols+rightImg.cols,rightImg.rows));  
//  
//    rightImg.copyTo(Mat(tiledImg,Rect(leftImg.cols,0,rightImg.cols,rightImg.rows)));  
//  
//   
//  
////保存图像  
//  
//    //imwrite("tiled.jpg",tiledImg);  
//  
//          
//  
//    //显示拼接的图像  
//  
//    imshow("tiled image",tiledImg);  
//  
//    waitKey(0);  
//  
//    return 0;  
//  
//}  