#include "stdafx.h"
#include <iostream>   
#include <iomanip>  
#include <opencv2\opencv.hpp>
#include "opencv2/core/core.hpp"  
#include "opencv2/objdetect/objdetect.hpp"   
#include "opencv2/features2d/features2d.hpp"  
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/calib3d/calib3d.hpp"  
#include "opencv2/imgproc.hpp"  
#include <opencv2\xfeatures2d.hpp>
  
using namespace cv;   
using namespace std;  
using namespace xfeatures2d;
bool refineMatchesWithHomography(const std::vector<cv::KeyPoint>& queryKeypoints,const std::vector<cv::KeyPoint>& trainKeypoints,  
float reprojectionThreshold,std::vector<cv::DMatch>& matches,cv::Mat& homography)  ;

bool IsRectCross(const Point &p1,const Point &p2,const Point &q1,const Point &q2)
{
    bool ret = min(p1.x,p2.x) <= max(q1.x,q2.x)    &&
                 min(q1.x,q2.x) <= max(p1.x,p2.x) &&
                 min(p1.y,p2.y) <= max(q1.y,q2.y) &&
                 min(q1.y,q2.y) <= max(p1.y,p2.y);
     return ret;
 }
 //跨立判断
 bool IsLineSegmentCross(const Point &pFirst1,const Point &pFirst2,const Point &pSecond1,const Point &pSecond2)
 {
     long line1,line2;
     line1 = pFirst1.x * (pSecond1.y - pFirst2.y) +
         pFirst2.x * (pFirst1.y - pSecond1.y) +
         pSecond1.x * (pFirst2.y - pFirst1.y);
     line2 = pFirst1.x * (pSecond2.y - pFirst2.y) +
         pFirst2.x * (pFirst1.y - pSecond2.y) + 
         pSecond2.x * (pFirst2.y - pFirst1.y);
     if (((line1 ^ line2) >= 0) && !(line1 == 0 && line2 == 0))
         return false;
 
     line1 = pSecond1.x * (pFirst1.y - pSecond2.y) +
         pSecond2.x * (pSecond1.y - pFirst1.y) +
         pFirst1.x * (pSecond2.y - pSecond1.y);
     line2 = pSecond1.x * (pFirst2.y - pSecond2.y) + 
         pSecond2.x * (pSecond1.y - pFirst2.y) +
         pFirst2.x * (pSecond2.y - pSecond1.y);
     if (((line1 ^ line2) >= 0) && !(line1 == 0 && line2 == 0))
         return false;
     return true;
 }
 
 bool GetCrossPoint(const Point &p1,const Point &p2,const Point &q1,const Point &q2,long &x,long &y)
 {
     if(IsRectCross(p1,p2,q1,q2))
     {
         if (IsLineSegmentCross(p1,p2,q1,q2))
         {
             //求交点
             long tmpLeft,tmpRight;
             tmpLeft = (q2.x - q1.x) * (p1.y - p2.y) - (p2.x - p1.x) * (q1.y - q2.y);
             tmpRight = (p1.y - q1.y) * (p2.x - p1.x) * (q2.x - q1.x) + q1.x * (q2.y - q1.y) * (p2.x - p1.x) - p1.x * (p2.y - p1.y) * (q2.x - q1.x);
 
             x = (int)((double)tmpRight/(double)tmpLeft);
 
             tmpLeft = (p1.x - p2.x) * (q2.y - q1.y) - (p2.y - p1.y) * (q1.x - q2.x);
             tmpRight = p2.y * (p1.x - p2.x) * (q2.y - q1.y) + (q2.x- p2.x) * (q2.y - q1.y) * (p1.y - p2.y) - q2.y * (q1.x - q2.x) * (p2.y - p1.y); 
             y = (int)((double)tmpRight/(double)tmpLeft);
             return true;
         }
     }
     return false;
 }

 bool IsPointInPolygon(std::vector<Point> poly,Point pt)
 {
     int i,j;
     bool c = false;
     for (i = 0,j = poly.size() - 1;i < poly.size();j = i++)
     {
         if ((((poly[i].y <= pt.y) && (pt.y < poly[j].y)) ||
             ((poly[j].y <= pt.y) && (pt.y < poly[i].y)))
             && (pt.x < (poly[j].x - poly[i].x) * (pt.y - poly[i].y)/(poly[j].y - poly[i].y) + poly[i].x))
         {
             c = !c;
         }
     }
     return c;
 }

 bool PointCmp(const Point &a,const Point &b,const Point &center)
 {
     if (a.x >= 0 && b.x < 0)
         return true;
     if (a.x == 0 && b.x == 0)
         return a.y > b.y;
     //向量OA和向量OB的叉积
     int det = (a.x - center.x) * (b.y - center.y) - (b.x - center.x) * (a.y - center.y);
     if (det < 0)
         return true;
     if (det > 0)
         return false;
     //向量OA和向量OB共线，以距离判断大小
     int d1 = (a.x - center.x) * (a.x - center.x) + (a.y - center.y) * (a.y - center.y);
     int d2 = (b.x - center.x) * (b.x - center.y) + (b.y - center.y) * (b.y - center.y);
     return d1 > d2;
 }
 void ClockwiseSortPoints(std::vector<Point> &vPoints)
 {
     //计算重心
     cv::Point center;
     double x = 0,y = 0;
     for (int i = 0;i < vPoints.size();i++)
     {
         x += vPoints[i].x;
         y += vPoints[i].y;
     }
     center.x = (int)x/vPoints.size();
     center.y = (int)y/vPoints.size();
 
     //冒泡排序
     for(int i = 0;i < vPoints.size() - 1;i++)
     {
         for (int j = 0;j < vPoints.size() - i - 1;j++)
         {
             if (PointCmp(vPoints[j],vPoints[j+1],center))
             {
                 cv::Point tmp = vPoints[j];
                 vPoints[j] = vPoints[j + 1];
                 vPoints[j + 1] = tmp;
             }
         }
     }
 }

 bool PolygonClip(const vector<cv::Point> &poly1,const vector<cv::Point> &poly2, std::vector<cv::Point> &interPoly)
 {
     if (poly1.size() < 3 || poly2.size() < 3)
   {
         return false;
     }
 
     long x,y;
     //计算多边形交点
     for (int i = 0;i < poly1.size();i++)
     {
         int poly1_next_idx = (i + 1) % poly1.size();
         for (int j = 0;j < poly2.size();j++)
         {
             int poly2_next_idx = (j + 1) % poly2.size();
             if (GetCrossPoint(poly1[i],poly1[poly1_next_idx],
                 poly2[j],poly2[poly2_next_idx],
                 x,y))
             {
                 interPoly.push_back(cv::Point(x,y));
             }
         }
     }
 
     //计算多边形内部点
     for(int i = 0;i < poly1.size();i++)
     {
         if (IsPointInPolygon(poly2,poly1[i]))
         {
             interPoly.push_back(poly1[i]);
         }
     }
     for (int i = 0;i < poly2.size();i++)
     {
         if (IsPointInPolygon(poly1,poly2[i]))
         {
             interPoly.push_back(poly2[i]);
         }
     }
     
     if(interPoly.size() <= 0)
         return false;
         
     //点集排序 
     ClockwiseSortPoints(interPoly);
     return true;
 }

bool ImageOverlap(cv::Mat &img1,cv::Mat &img2,std::vector<cv::Point> &vPtsImg1,std::vector<cv::Point> &vPtsImg2)
 {
    cv::Mat g1(img1,Rect(0,0,img1.cols,img1.rows));
    cv::Mat g2(img2,Rect(0,0,img2.cols,img2.rows));

    cv::cvtColor(g1,g1,CV_BGR2GRAY);
    cv::cvtColor(g2,g2,CV_BGR2GRAY);
 
    std::vector<cv::KeyPoint> keypoints_roi, keypoints_img;  /* keypoints found using SIFT */
      cv::Mat descriptor_roi, descriptor_img;             /* Descriptors for SIFT */
      cv::FlannBasedMatcher matcher;                      /* FLANN based matcher to match keypoints */
      std::vector<cv::DMatch> matches, good_matches;
	  //Ptr<FeatureDetector> detector = FastFeatureDetector::create(40);
	  Ptr<FeatureDetector> detector = SURF::create(400);
	  //Ptr<FeatureDetector> detector = SIFT::create();
      int i, dist=80;
  
      //sift->detect(g1,keypoints_roi, descriptor_roi);      /* get keypoints of ROI image */
      //sift->detect(g2, keypoints_img, descriptor_img);         /* get keypoints of the image */

	  detector->detect( g1, keypoints_roi );
	  detector->detect( g2, keypoints_img );

	  Ptr<SURF>extractor = SURF::create();
	  extractor->compute( g1, keypoints_roi, descriptor_roi );
	  extractor->compute( g2, keypoints_img, descriptor_img );

      matcher.match(descriptor_roi, descriptor_img, matches);
  
      double max_dist = 0; double min_dist = 1000;
  
      //-- Quick calculation of max and min distances between keypoints
      /*for( int i = 0; i < matches.size(); i++ )
      { 
          double dist = matches[i].distance;
          if( dist < min_dist ) min_dist = dist;
          if( dist > max_dist ) max_dist = dist;
      }
  
      for (i=0; i < descriptor_roi.rows; i++)
      {
          if (matches[i].distance < 3*min_dist)
          {
              good_matches.push_back(matches[i]);
          }
      }*/
  
      //printf("%ld no. of matched keypoints in right image\n", good_matches.size());
      /* Draw matched keypoints */
  
      //Mat img_matches;
      //drawMatches(img1, keypoints_roi, img2, keypoints_img, 
      //    good_matches, img_matches, Scalar::all(-1), 
      //    Scalar::all(-1), vector<char>(), 
      //    DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
      //imshow("matches",img_matches);
  
	  refineMatchesWithHomography(keypoints_roi, keypoints_img, 3.0, matches, Mat());
	  Mat img_matches;
	  drawMatches( g1, keypoints_roi, g2, keypoints_img,
               matches, img_matches, Scalar::all(-1), Scalar::all(-1),
               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
	  imshow( "匹配成功", img_matches );
  waitKey(0);
	  std::vector<Point2f> keypoints1;
	  std::vector<Point2f> keypoints2; 
      for (i=0; i<matches.size(); i++)
      {
          keypoints1.push_back(keypoints_img[matches[i].trainIdx].pt);
          keypoints2.push_back(keypoints_roi[matches[i].queryIdx].pt);
      }
      //计算单应矩阵
      Mat H = findHomography( keypoints1, keypoints2, CV_RANSAC );
  
      //show stitchImage
      // cv::Mat stitchedImage;
      // int mRows = img2.rows;
      // if (img1.rows> img2.rows)
      // {
          // mRows = img1.rows;
      // }
      // stitchedImage = Mat::zeros(img2.cols+img1.cols, mRows, CV_8UC3);
      // warpPerspective(img2,stitchedImage,H,Size(img2.cols+img1.cols,mRows));
      // Mat half(stitchedImage,Rect(0,0,img1.cols,img1.rows));
      // img1.copyTo(half);
      // imshow("stitchedImage",stitchedImage);
  
      std::vector<cv::Point> vSrcPtsImg1;
      std::vector<cv::Point> vSrcPtsImg2;
  
      vSrcPtsImg1.push_back(cv::Point(0,0));
      vSrcPtsImg1.push_back(cv::Point(0,img1.rows));
      vSrcPtsImg1.push_back(cv::Point(img1.cols,img1.rows));
      vSrcPtsImg1.push_back(cv::Point(img1.cols,0));
  
      vSrcPtsImg2.push_back(cv::Point(0,0));
      vSrcPtsImg2.push_back(cv::Point(0,img2.rows));
      vSrcPtsImg2.push_back(cv::Point(img2.cols,img2.rows));
      vSrcPtsImg2.push_back(cv::Point(img2.cols,0));
  
      //计算图像2在图像1中对应坐标信息
      std::vector<cv::Point> vWarpPtsImg2;
      for(int i = 0;i < vSrcPtsImg2.size();i++ )
      {
          cv::Mat srcMat = Mat::zeros(3,1,CV_64FC1);
          srcMat.at<double>(0,0) = vSrcPtsImg2[i].x;
          srcMat.at<double>(1,0) = vSrcPtsImg2[i].y;
          srcMat.at<double>(2,0) = 1.0;
          
          cv::Mat warpMat = H * srcMat;
          cv::Point warpPt;
          warpPt.x = cvRound(warpMat.at<double>(0,0)/warpMat.at<double>(2,0));
          warpPt.y = cvRound(warpMat.at<double>(1,0)/warpMat.at<double>(2,0));
  
          vWarpPtsImg2.push_back(warpPt);
      }
      //计算图像1和转换后的图像2的交点
     if(!PolygonClip(vSrcPtsImg1,vWarpPtsImg2,vPtsImg1))
         return false;
 
     for (int i = 0;i < vPtsImg1.size();i++)
     {
         cv::Mat srcMat = Mat::zeros(3,1,CV_64FC1);
         srcMat.at<double>(0,0) = vPtsImg1[i].x;
         srcMat.at<double>(1,0) = vPtsImg1[i].y;
         srcMat.at<double>(2,0) = 1.0;
 
         cv::Mat warpMat = H.inv() * srcMat;
         cv::Point warpPt;
         warpPt.x = cvRound(warpMat.at<double>(0,0)/warpMat.at<double>(2,0));
         warpPt.y = cvRound(warpMat.at<double>(1,0)/warpMat.at<double>(2,0));
         vPtsImg2.push_back(warpPt);
     }
	 for(int i = 0; i < vPtsImg2.size(); i++){
		 cout << vPtsImg2[i].x << " " << vPtsImg2[i].y << endl;
	 }
     return true;
 }

 //void main(){
	// Mat img1 = imread("E:\\screenshots\\tempNULL\\13.jpg");
	// Mat img2 = imread("E:\\screenshots\\tempNULL\\14.jpg");
	// /*Mat img1 = imread("left.jpg");
	// Mat img2 = imread("right.jpg");*/
	// std::vector<cv::Point> vPtsImg1;
	// std::vector<cv::Point> vPtsImg2;

	// if(ImageOverlap(img1, img2, vPtsImg1, vPtsImg2)){
	//	 imshow("test", img1);
	//	 waitKey(0);
	// }
 //}
