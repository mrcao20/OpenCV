#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>

using namespace std;
using namespace cv;

//int main(int argc, char** argv){
//  // can change to BOOSTING, MIL, KCF (OpenCV 3.1), TLD, MEDIANFLOW, or GOTURN (OpenCV 3.2)
//  //Ptr<Tracker> tracker = TrackerBoosting::create();
//  Ptr<Tracker> tracker = TrackerMIL::create();
//  //Ptr<Tracker> tracker = TrackerMedianFlow::create();
//  //Ptr<Tracker> tracker = TrackerKCF::create();
//  //VideoCapture video(0);
//  VideoCapture video("E:\\machineLearning\\video\\1 - 1 - Welcome (7 min).mkv");
//  if(!video.isOpened()){
//    cerr << "cannot read video!" << endl;
//    return -1;
//  }
//
//  Mat frame;
//  //video.read(frame);
//  frame = imread("E:\\screenshots\\source\\0.jpg", 1);  
//  //frame = imread("pic\\temp\\0.jpg", 1);  
//  Rect2d box = selectROI("Tracking", frame);
//  //Rect2d box(270, 120, 180, 260);
//  tracker->init(frame, box);
// /* while(video.read(frame)){
//    tracker->update(frame, box);
//    rectangle(frame, box, Scalar(255, 0, 0), 2, 1);
//    imshow("Tracking", frame);
//    int k=waitKey(1);
//    if(k==27) break;
//  }*/
//  for(int i = 0; i <= 67; i++)
//	{
//		char temp[8];
//		sprintf_s(temp, "%d.jpg", i);
//		string str = temp;
//		frame = imread("E:\\screenshots\\source\\" + str, 1);  
//		//frame = imread("pic\\temp\\" + str, 1);  
//		tracker->update(frame, box);
//		rectangle(frame, box, Scalar(255, 0, 0), 2, 1);
//		imshow("img",frame);  
//		cv::waitKey(1);  
//	}
//}