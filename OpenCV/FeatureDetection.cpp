#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/core/ocl.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgproc.hpp"
#include"opencv2/flann.hpp"
#include"opencv2/ml.hpp"
#include <opencv2\xfeatures2d.hpp>
#include <windows.h>

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;

int matchesThreshold = 20;
int featureNum = 0;
extern int roomNum;
extern string pictureName;

void readme();
Point2f find(Mat& img_object1, Mat& img_object2, Mat& img_scene);
Mat surf(int i, Mat& img_object, Mat& img_scene);
bool refineMatchesWithHomography(const std::vector<cv::KeyPoint>& queryKeypoints,const std::vector<cv::KeyPoint>& trainKeypoints,  
	float reprojectionThreshold,std::vector<cv::DMatch>& matches,cv::Mat& homography);
Point2f findPoint(Mat& img_object, Mat& img_matches, Mat& homography, int i);
Point2f find(Mat& img_character, Mat& img_scene);

//int main( int argc, char** argv )
//{
//	SYSTEMTIME start;
//	GetLocalTime(&start);
//
//	/*Mat img_object1 = imread("pic\\target\\leftCloseDoor.jpg", CV_LOAD_IMAGE_GRAYSCALE);
//	Mat img_object2 = imread("E:\\screenshots\\1\\234.jpg", CV_LOAD_IMAGE_GRAYSCALE);
//	Point2f point1 = find(img_object1, img_object2);
//
//	img_object1 = imread("pic\\target\\left.jpg", CV_LOAD_IMAGE_GRAYSCALE);
//	img_object2 = imread("E:\\screenshots\\1\\234.jpg", CV_LOAD_IMAGE_GRAYSCALE);
//	Point2f point2 = find(img_object1, img_object2);
//	cout << point2 - point1 << endl;*/
//	Mat img_object1 = imread("pic\\target\\oneRightDoor.jpg", CV_LOAD_IMAGE_GRAYSCALE);
//	Mat img_object2 = imread("E:\\screenshots\\1\\25.jpg", CV_LOAD_IMAGE_GRAYSCALE);
//	cout << find(img_object1, img_object2) << endl;
//
//	SYSTEMTIME end;
//	GetLocalTime(&end);
//	cout << (end.wMinute - start.wMinute) * 60 + (end.wSecond - start.wSecond) * 1000 + (end.wMilliseconds - start.wMilliseconds) << "ms" << endl;
//	
//  return 0;
//  
//}

Point2f find(Mat& img_door, Mat& img_character, Mat& img_scene)
{
	Mat H1, H2;
	Point2f img_door_point, img_character_point;
	H1 = surf(0, img_door, img_scene);
	H2 = surf(0, img_character, img_scene);
	if(!H1.data || !H2.data){
		return Point2f(-1, -1);
	}
	img_door_point = findPoint(img_door, img_scene, H1, 0);
	img_character_point = findPoint(img_character, img_scene, H2, 1);
	//cout << img_door_point << " " << img_character_point << endl;
	/*cout << img_door_point - img_character_point << endl;
	line( img_scene, img_door_point, img_character_point, Scalar( 0, 255, 0), 4 );
	imshow( "标注", img_scene );
	waitKey(0);*/
	return img_door_point - img_character_point;
}

Point2f find(Mat& img_character, Mat& img_scene)
{
	Mat H1;
	Point2f img_character_point;
	H1 = surf(0, img_character, img_scene);
	if(!H1.data){
		return Point2f(-1, -1);
	}
	img_character_point = findPoint(img_character, img_scene, H1, 1);
	return img_character_point;
}

bool refineMatchesWithHomography(const std::vector<cv::KeyPoint>& queryKeypoints,const std::vector<cv::KeyPoint>& trainKeypoints,  
float reprojectionThreshold,std::vector<cv::DMatch>& matches,cv::Mat& homography)  
{  
    const int minNumberMatchesAllowed = 8;  
    if (matches.size() < minNumberMatchesAllowed)  
        return false;  
    // Prepare data for cv::findHomography    
    std::vector<cv::Point2f> srcPoints(matches.size());  
    std::vector<cv::Point2f> dstPoints(matches.size());  
    for (size_t i = 0; i < matches.size(); i++)  
    {  
        srcPoints[i] = trainKeypoints[matches[i].trainIdx].pt;  
        dstPoints[i] = queryKeypoints[matches[i].queryIdx].pt;  
        //srcPoints[i] = trainKeypoints[i].pt;  
        //dstPoints[i] = queryKeypoints[i].pt;  
    }  
    // Find homography matrix and get inliers mask    
    std::vector<unsigned char> inliersMask(srcPoints.size());  
    homography = cv::findHomography(srcPoints,dstPoints,CV_FM_RANSAC,reprojectionThreshold,inliersMask);  
    std::vector<cv::DMatch> inliers;  
    for (size_t i = 0; i<inliersMask.size(); i++)  
    {  
        if (inliersMask[i])  
            inliers.push_back(matches[i]);  
    }  
    matches.swap(inliers);  
    return matches.size() > minNumberMatchesAllowed;  
}  

Mat surf(int i, Mat& img_object, Mat& img_scene)
{
	if( !img_object.data || !img_scene.data )
	{ std::cout<< " --(!) Error reading images " << std::endl; return Mat(); }

  //-- Step 1: Detect the keypoints using SURF Detector

  Ptr<FeatureDetector> detector = FastFeatureDetector::create(20);
  /*if(i == 0)
  {
	  int minHessian = 40;
	detector = FastFeatureDetector::create(minHessian);
	cout << "FAST" <<endl;
  }
  else
  {
	  int minHessian = 400;
	detector = SURF::create(minHessian);
	cout << "SURF" <<endl;
  }*/
  
  std::vector<KeyPoint> keypoints_object, keypoints_scene;
  detector->detect( img_object, keypoints_object );
  detector->detect( img_scene, keypoints_scene );

  //-- Step 2: Calculate descriptors (feature vectors)
  Ptr<SURF>extractor = SURF::create();

  Mat descriptors_object, descriptors_scene;
  
  extractor->compute( img_object, keypoints_object, descriptors_object );
  extractor->compute( img_scene, keypoints_scene, descriptors_scene );
  
  //-- Step 3: Matching descriptor vectors using FLANN matcher
  FlannBasedMatcher matcher;
  //BFMatcher matcher(NORM_L2);
  std::vector< DMatch > matches;
  matcher.match( descriptors_object, descriptors_scene, matches );
  
  //double max_dist = 0; double min_dist = FLT_MAX;

  ////-- Quick calculation of max and min distances between keypoints
  //for( int i = 0; i < matches.size(); i++ )
  //{ double dist = matches[i].distance;
  //  if( dist < min_dist ) min_dist = dist;
  //  if( dist > max_dist ) max_dist = dist;
  //}

  //printf("-- Max dist : %f \n", max_dist );
  //printf("-- Min dist : %f \n", min_dist );

  //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
  //std::vector< DMatch > good_matches;
  Mat homography;
  
  //cout << refineMatchesWithHomography(keypoints_object, keypoints_scene, 2.0, matches, homography) << endl;
  refineMatchesWithHomography(keypoints_object, keypoints_scene, 2.0, matches, homography);

  if(matches.size() < matchesThreshold){
	  return Mat();
  }
  featureNum = matches.size();
  cout << "roomNum:" << roomNum << " " << pictureName << ":" << featureNum << endl;

  /*for( int i = 0; i < descriptors_object.rows; i++ )
  { if( matches[i].distance < 2*min_dist )
     { good_matches.push_back( matches[i]); }
  }*/

  /*Mat img_matches;
  drawMatches( img_object, keypoints_object, img_scene, keypoints_scene,
               matches, img_matches, Scalar::all(-1), Scalar::all(-1),
               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
  imshow( "匹配成功", img_matches );
  waitKey(0);*/
  
  std::vector<Point2f> obj;
  std::vector<Point2f> scene;

  for( int i = 0; i < matches.size(); i++ )
  {
    //-- Get the keypoints from the good matches
    obj.push_back( keypoints_object[ matches[i].queryIdx ].pt );
    scene.push_back( keypoints_scene[ matches[i].trainIdx ].pt );
  }
  
  Mat H = findHomography( obj, scene, CV_RANSAC );

  return H;
}

Point2f findPoint(Mat& img_object, Mat& img_scene, Mat& homography, int i)
{
	//-- Get the corners from the image_1 ( the object to be "detected" )
  std::vector<Point2f> obj_corners(4);
  obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( img_object.cols, 0 );
  obj_corners[2] = cvPoint( img_object.cols, img_object.rows ); obj_corners[3] = cvPoint( 0, img_object.rows );
  std::vector<Point2f> scene_corners(4);
  
  perspectiveTransform( obj_corners, scene_corners, homography);
  
  //-- Draw lines between the corners (the mapped object in the scene - image_2 )
  /*line( img_matches, scene_corners[0] + Point2f( img_object.cols, 0), scene_corners[1] + Point2f( img_object.cols, 0), Scalar(0, 255, 0), 4 );
  line( img_matches, scene_corners[1] + Point2f( img_object.cols, 0), scene_corners[2] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );
  line( img_matches, scene_corners[2] + Point2f( img_object.cols, 0), scene_corners[3] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );
  line( img_matches, scene_corners[3] + Point2f( img_object.cols, 0), scene_corners[0] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );*/

  /*line( img_matches, scene_corners[0], scene_corners[1], Scalar(0, 255, 0), 4 );
  line( img_matches, scene_corners[1], scene_corners[2], Scalar( 0, 255, 0), 4 );
  line( img_matches, scene_corners[2], scene_corners[3], Scalar( 0, 255, 0), 4 );
  line( img_matches, scene_corners[3], scene_corners[0], Scalar( 0, 255, 0), 4 );*/

  /*line( img_matches, scene_corners[0], scene_corners[2], Scalar( 0, 255, 0), 4 );
  line( img_matches, scene_corners[1], scene_corners[3], Scalar( 0, 255, 0), 4 );*/
  if(i == 0)
  {
	 scene_corners[2] -= Point2f(0, 21);
	 scene_corners[3] -= Point2f(0, 21);
  }
  //line( img_scene, scene_corners[2], scene_corners[3], Scalar( 0, 255, 0), 4 );
  //cout << sqrtf(pow((scene_corners[2].x - scene_corners[3].x), 2) + pow((scene_corners[2].y - scene_corners[3].y), 2)) << endl;

  //-- Show detected matches
  /*imshow( "标注", img_scene );
  waitKey(0);*/
  return (scene_corners[2] + scene_corners[3]) / 2;
}

  /** @function readme */
  void readme()
  { std::cout << " Usage: ./SURF_descriptor <img1> <img2>" << std::endl; }