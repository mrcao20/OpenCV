#include "stdafx.h"
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <cstdio>
using namespace std;
using namespace cv;

void test();
void motionToColor(Mat flow, Mat &color) ;
//int main(int, char**)  
//{ 
//    test();
//    return 0;  
//}  

void test(){
	Mat prevgray, gray, flow, cflow, frame;  
    namedWindow("flow", 1);  
  
    Mat motion2color;  

    for(int j = 0; j <= 356; j++)  
    {  
        double t = (double)cvGetTickCount();  
  
		char temp[8];
		sprintf_s(temp, "%d.jpg", j);
		string str = temp;
        //cap >> frame;  
		Mat frame = imread("E:\\screenshots\\tempNULL\\" + str);
		if(!frame.data) { cout << "Í¼Æ¬¶ÁÈ¡Ê§°Ü" << endl; break;}
		//frame = Mat(frame, Rect(0, 206, 795, 313));
        cvtColor(frame, gray, CV_BGR2GRAY);  
        //imshow("original", frame);  

		vector<Vec4f> position;
		vector<Point2f> status;
		float Threshold = 0.1;
		int offset = 10;
        if( prevgray.data )  
        {  
            calcOpticalFlowFarneback(prevgray, gray, flow, 0.5, 3, 15, 3, 5, 1.2, 0);  
			motionToColor(flow, motion2color); 
			for(int y = 0; y < flow.rows; y++){
				for(int x = 0; x < flow.cols; x++){
					Point2f flowAtPoint = flow.at<Point2f>(y, x);
					if(abs(flowAtPoint.x) + abs(flowAtPoint.y) > 1){
						if(status.empty()){
							status.push_back(flowAtPoint);
							position.push_back(Vec4f(x, y, x, y));
						}else{
							int i = 0;
							for(i = 0; i < status.size(); i++){
								if(status[i].x - flowAtPoint.x < Threshold && status[i].y - flowAtPoint.y < Threshold){
									if(position[i][0] - x > offset || y - position[i][3] > offset || x - position[i][2] > offset){
										continue;
									}
									if(x < position[i][0]){
										position[i][0] = x;
									}else if(x > position[i][2]){
										position[i][2] = x;
									}
									if(y > position[i][3]){
										position[i][3] = y;
									}
									break;
								}
							}
							if(i == status.size()){
								status.push_back(flowAtPoint);
								position.push_back(Vec4f(x, y, x, y));
							}
						}
					}
				}
			}
			for(int i = 0; i < position.size(); i++){
				rectangle(frame, Point(position[i][0], position[i][1]), Point(position[i][2], position[i][3]),Scalar(i*10, 255, i));
			}
			//cout << flow << endl;
            imshow("flow", motion2color);  
			imshow("dst", frame); 
        }  
        std::swap(prevgray, gray);  
  
        //t = (double)cvGetTickCount() - t;  
        //std::cout << "cost time: " << t / ((double)cvGetTickFrequency()*1000.) << endl;  
		if(waitKey(0) == 27){
			break;
		}
    }  
}