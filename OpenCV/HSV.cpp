#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2\imgcodecs.hpp>
#include <opencv2\cvconfig.h>
#include <iostream>
#include <time.h>

using namespace std;
using namespace cv;

void HSV();
void HLSTest();
void DrawRect(Mat &image);
void findCharactor(Mat &charactor, Mat &scene);

//void main(){
//	//HSV();
//	HLSTest();
//}

void findCharactor(Mat &charactor, Mat &scene){
	
}

void HLSTest(){
	Mat image = imread("E:\\JavaProgram\\AI\\images\\temp\\0.jpg");
	/*Mat image2 = imread("pic\\temp\\114.jpg");
	if(!image.data || !image2.data){
		cout << "Í¼Æ¬¶ÁÈëÊ§°Ü" << endl;
		return;
	}*/
	//image = Mat(image, Rect(0, 206, 795, 313));
	//cvtColor(image2, image2, COLOR_BGR2YCrCb);
	//imshow("src", image);
	imshow("HLS Image2", image);
	cvtColor(image, image, COLOR_RGB2BGR);

	//findCharactor(image2, image);

	//GaussianBlur(image3,image3, Size(3, 3), 0, 0);
	vector<Mat> mv;
	//dilate(image, image, getStructuringElement(0, Size(3, 3)));
	//erode(image, image, getStructuringElement(0, Size(3, 3)));
	//split(image, mv);
	////DrawRect(mv[2]);
	//imshow("H", mv[0]);
	//imshow("L", mv[1]);
	//imshow("S", mv[2]);
	//imshow("HLS Image2", image2);
	imshow("HLS", image);

	waitKey();
}

void DrawRect(Mat &image){
	std::vector< std::vector< cv::Point> > contours;  
	vector<Vec4i> hierarchy;
	findContours(image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	for(int j = 0; j < contours.size(); j++){
		Rect rect = boundingRect(contours[j]);
		rectangle(image, rect, Scalar(0, 255, 255));
	}
}

void HSV(){
	Mat image = imread("E:\\screenshots\\temp\\63.jpg");
	image = Mat(image, Rect(0, 206, 795, 313));
	imshow("src", image);
	clock_t start = clock();
	Mat image2;
	cvtColor(image, image2, CV_BGR2HSV);
	//GaussianBlur(image2,image2, Size(3, 3), 0, 0);
	//std::vector< std::vector< cv::Point> > contours;  
	/*findContours(image2, contours, noArray(), RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	drawContours(image2, contours, -1, Scalar(255));*/
	cout << (double)(clock() - start) / CLOCKS_PER_SEC * 1000 << endl;
	imshow("dst", image2);
	start = clock();
	Mat image3;
	cvtColor(image, image3, CV_RGB2HLS);
	//GaussianBlur(image3,image3, Size(3, 3), 0, 0);
	cout << (double)(clock() - start) / CLOCKS_PER_SEC * 1000 << endl;
	imshow("dst2", image3);

	waitKey();
}