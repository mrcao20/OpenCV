#include "stdafx.h"
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2\imgproc.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2\xfeatures2d.hpp>
#include "opencv2/features2d/features2d.hpp"

using namespace std;
using namespace cv;  
using namespace cv::xfeatures2d;

vector<Point2f> surf(int i, Mat& img_object, Mat& img_scene);
vector<Point2f> refineMatchesWithHomography(const std::vector<cv::KeyPoint>& queryKeypoints,const std::vector<cv::KeyPoint>& trainKeypoints,  
float reprojectionThreshold,std::vector<cv::DMatch>& matches,cv::Mat& homography)  ;

class FrameProcessor{  
    public:  
        virtual void process(Mat &input,Mat &ouput)=0;  
};  
  
//特征跟踪类，继承自帧处理基类  
class FeatureTracker :  public FrameProcessor{  
    Mat gray;  //当前灰度图  
    Mat gray_prev;  //之前的灰度图  
    vector<Point2f> points[2];//前后两帧的特征点  
    vector<Point2f> initial;//初始特征点  
    vector<Point2f> features;//检测到的特征  
    int max_count; //要跟踪特征的最大数目  
    double qlevel; //特征检测的指标  
    double minDist;//特征点之间最小容忍距离  
    vector<uchar> status; //特征点被成功跟踪的标志  
    vector<float> err; //跟踪时的特征点小区域误差和  
public:  
    FeatureTracker():max_count(500),qlevel(0.01),minDist(10.){}  
    void process(Mat &frame,Mat &output){  
        //得到灰度图  
        cvtColor (frame,gray,CV_BGR2GRAY);  
        frame.copyTo (output);  
        //特征点太少了，重新检测特征点  
        if(addNewPoint()){  
            detectFeaturePoint ();  
            //插入检测到的特征点  
            points[0].insert (points[0].end (),features.begin (),features.end ());  
            initial.insert (initial.end (),features.begin (),features.end ());  
        }  
        //第一帧  
        if(gray_prev.empty ()){  
                gray.copyTo (gray_prev);  
        }  
        //根据前后两帧灰度图估计前一帧特征点在当前帧的位置  
        //默认窗口是15*15  
        calcOpticalFlowPyrLK (  
                gray_prev,//前一帧灰度图  
                gray,//当前帧灰度图  
                points[0],//前一帧特征点位置  
                points[1],//当前帧特征点位置  
                status,//特征点被成功跟踪的标志  
                err);//前一帧特征点点小区域和当前特征点小区域间的差，根据差的大小可删除那些运动变化剧烈的点  
        int k = 0;  
        //去除那些未移动的特征点  
        for(int i=0;i<points[1].size ();i++){  
            if(acceptTrackedPoint (i)){  
                initial[k]=initial[i];  
                points[1][k++] = points[1][i];  
            }  
        }  
        points[1].resize (k);  
        initial.resize (k);  
        //标记被跟踪的特征点  
        handleTrackedPoint (frame,output);  
        //为下一帧跟踪初始化特征点集和灰度图像  
        std::swap(points[1],points[0]);  
        cv::swap(gray_prev,gray);  
    }  
  
    void detectFeaturePoint(){  
        //goodFeaturesToTrack (gray,//输入图片  
        //                         features,//输出特征点  
        //                         max_count,//特征点最大数目  
        //                         qlevel,//质量指标  
        //                         minDist);//最小容忍距离  
		Mat frame = imread("E:\\screenshots\\tempNULL\\0.jpg", 1);  
		Mat charactor = imread("pic\\temp\\target1.jpg", 1);  
		features = surf(0, charactor, frame);
    }  
    bool addNewPoint(){  
        //若特征点数目少于10，则决定添加特征点  
		cout << points[0].size () << endl;
        return points[0].size ()<=10;  
    }  
  
    //若特征点在前后两帧移动了，则认为该点是目标点，且可被跟踪  
    bool acceptTrackedPoint(int i){  
        return status[i]&&  
                (abs(points[0][i].x-points[1][i].x)+  
                  abs(points[0][i].y-points[1][i].y) >2);  
    }  
  
    //画特征点  
    void  handleTrackedPoint(Mat &frame,Mat &output){  
            for(int i=0;i<points[i].size ();i++){  
                //当前特征点到初始位置用直线表示  
                line(output,initial[i],points[1][i],Scalar::all (0));  
                //当前位置用圈标出  
                circle(output,points[1][i],3,Scalar::all(0),(-1));  
            }  
        }  
};
//
//void main(){
//	FeatureTracker f;
//	for(int i = 0; i <= 65; i++)
//	{
//		char temp[8];
//		sprintf_s(temp, "%d.jpg", i);
//		string str = temp;
//		Mat frame = imread("E:\\screenshots\\tempNULL\\" + str, 1);  
//		Mat output;
//		f.process(frame, output);
//		imshow("mask",output);  
//        waitKey(0); 
//	}
//}

vector<cv::Point2f> refineMatchesWithHomography(const std::vector<cv::KeyPoint>& queryKeypoints,const std::vector<cv::KeyPoint>& trainKeypoints,  
float reprojectionThreshold,std::vector<cv::DMatch>& matches,cv::Mat& homography)  
{  
    const int minNumberMatchesAllowed = 8;  
    if (matches.size() < minNumberMatchesAllowed)  
        return vector<cv::Point2f>();  
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
    return srcPoints;
}  

vector<cv::Point2f> surf(int i, Mat& img_object, Mat& img_scene)
{
	if( !img_object.data || !img_scene.data )
	{ std::cout<< " --(!) Error reading images " << std::endl; return Mat(); }

  //-- Step 1: Detect the keypoints using SURF Detector
  
  Ptr<FeatureDetector> detector;
  if(i == 0)
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
  }
  
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
  std::vector<cv::Point2f> dstPoints(matches.size());
  dstPoints = refineMatchesWithHomography(keypoints_object, keypoints_scene, 2.0, matches, homography);
  /*for( int i = 0; i < descriptors_object.rows; i++ )
  { if( matches[i].distance < 2*min_dist )
     { good_matches.push_back( matches[i]); }
  }*/
  Mat img_matches;
  drawMatches( img_object, keypoints_object, img_scene, keypoints_scene,
               matches, img_matches, Scalar::all(-1), Scalar::all(-1),
               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
  imshow( "匹配成功", img_matches );
  waitKey(0);
  //-- Localize the object
  std::vector<Point2f> obj;
  std::vector<Point2f> scene;

  for( int i = 0; i < matches.size(); i++ )
  {
    //-- Get the keypoints from the good matches
    obj.push_back( keypoints_object[ matches[i].queryIdx ].pt );
    scene.push_back( keypoints_scene[ matches[i].trainIdx ].pt );
  }
  
  Mat H = findHomography( obj, scene, CV_RANSAC );

  return dstPoints;
}