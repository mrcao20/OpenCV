#include "stdafx.h"
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <cstdio>
using namespace std;
using namespace cv;
//  描述：声明全局函数
Point2f tracking(Mat &frame);
bool addNewPoints();
bool acceptTrackedPoint(int i);
void tracker();
//  描述：声明全局变量
string window_name = "optical flow tracking";
Mat gray;   // 当前图片
Mat gray_prev;  // 预测图片
vector<Point2f> points[2];  // point0为特征点的原来位置，point1为特征点的新位置
//vector<Point2f> initial;    // 初始化跟踪点的位置
vector<Point2f> features;   // 检测的特征
int maxCount = 500; // 检测的最大特征数
double qLevel = 0.01;   // 特征检测的等级
double minDist = 10.0;  // 两特征点之间的最小距离
vector<uchar> status;   // 跟踪特征的状态，特征的流发现为1，否则为0
vector<float> err;
int picNum = 0;
//输出相应信息和OpenCV版本-----
static void helpinformation()
{
    cout <<"\n\n\t\t\t 光流法跟踪运动目标检测\n"
         <<"\n\n\t\t\t 当前使用的OpenCV版本为：" << CV_VERSION 
         <<"\n\n" ;
}

//int main()
//{
//	tracker();
//    return 0;
//}

void tracker(){
	Mat frame;
    //显示信息函数
    helpinformation();
    for(int i = 62; i <= 356; i++)
    {
		stringstream strStream;
		strStream << i;
		string str;
		strStream >> str;
		frame = imread("E:\\screenshots\\tempNULL\\" + str + ".jpg");
		//cvtColor(frame,frame,COLOR_BGR2HLS);
        if(!frame.empty())
        { 
            tracking(frame);
        }
        else
        { 
            printf(" --(!) No captured frame -- Break!");
            break;
        }
        int c = waitKey(0);
        if( (char)c == 27 )
        {
            break; 
        } 
    }
}

Point2f drawRect(Mat &output){
	float threshold = 50.f;
	vector<vector<float>> point(1);
	vector<int> density(1);
	point[0].push_back(points[1][0].x);
	point[0].push_back(points[1][0].y);
	point[0].push_back(points[1][0].x);
	point[0].push_back(points[1][0].y);
	density[0] = 1;
	for(int i = 1; i < points[1].size(); i++){
		if(points[1][i].y < 120 || points[1][i].y > 525){
			continue;
		}

		int j;
		for(j = 0; j < point.size(); j++){
			if(!(points[1][i].x-point[j][0]<-threshold || points[1][i].x-point[j][2]>threshold 
				|| points[1][i].y-point[j][1]<-threshold || points[1][i].y-point[j][3]>threshold)){
				point[j][0] = points[1][i].x < point[j][0] ? points[1][i].x : point[j][0];
				point[j][1] = points[1][i].y < point[j][1] ? points[1][i].y : point[j][1];
				point[j][2] = points[1][i].x > point[j][2] ? points[1][i].x : point[j][2];
				point[j][3] = points[1][i].y > point[j][3] ? points[1][i].y : point[j][3];
				density[j]++;
				break;
			}
		}
		if(j == point.size()){
			vector<float> tempPoint;
			tempPoint.push_back(points[1][i].x);
			tempPoint.push_back(points[1][i].y);
			tempPoint.push_back(points[1][i].x);
			tempPoint.push_back(points[1][i].y);
			point.push_back(tempPoint);
			density.push_back(1);
		}
	}
	if(point.size() > 3){
		return Point2f(-1, -1);
	}

	int index = 0;
	for(int i = 1; i < point.size(); i++){
		if(density[i] > density[index]){
			index = i;
		}
	}
	rectangle(output, Point2f(point[index][0], point[index][1]), Point2f(point[index][2], point[index][3]), Scalar(0, 255, 255));
	//cout << Point2f((point[index][2] - point[index][0]) / 2, point[index][3]) << endl;
	cout << "特征点数量：" << density[index] << endl;
	if(density[index] < 6 || point[index][2] - point[index][0] > 200 || point[index][3] - point[index][1] > 200 || point[index][3] > 525 || point[index][3] < 120 || 
		point[index][2] - point[index][0] < 20 || point[index][3] - point[index][1] < 20){
		cout << "特征量不满足条件" << endl;
		return Point2f(-1, -1);
	}
	for (size_t i=0; i<points[1].size(); i++)
    {
		//line(output, initial[i], points[1][i], Scalar(0, 0, 255));
		line(output, points[0][i], points[1][i], Scalar(255, 255, 0));
		circle(output, points[1][i], 3, Scalar(0, 255, 0), -1);
    }
	//stringstream ss;
	//ss << picNum;
	//imwrite("E:\\screenshots\\a\\" + ss.str() + ".jpg", output);
	//picNum++;
	return Point2f((point[index][2] + point[index][0]) / 2, point[index][3]);
}

// parameter: frame 输入的视频帧
//            output 有跟踪结果的视频帧
Point2f tracking(Mat &frame)
{
	Mat output;
    cvtColor(frame, gray, CV_BGR2GRAY);
    frame.copyTo(output);
	vector<Point2f> tempP;

	if (gray_prev.empty())
    {
        gray.copyTo(gray_prev);
    }

    // 添加特征点
    if (addNewPoints())
    {
        goodFeaturesToTrack(gray_prev, features, maxCount, qLevel, minDist);
		points[0].insert(points[0].begin(), features.begin(), features.end());
        //initial.insert(initial.end(), features.begin(), features.end());

		//tempP.insert(tempP.begin(), features.begin(), features.end());
	}

	/*goodFeaturesToTrack(gray, features, maxCount, qLevel, minDist);
	tempP.insert(tempP.begin(), features.begin(), features.end());*/
	
	/*Mat temp;
	output.copyTo(temp);
	for (size_t i=0; i<tempP.size(); i++)
    {
        circle(temp, tempP[i], 3, Scalar(0, 255, 0), -1);
    }
	imshow("test", temp);*/
	//waitKey();
    // l-k光流法运动估计
    calcOpticalFlowPyrLK(gray_prev, gray, points[0], points[1], status, err);
    // 去掉一些不好的特征点
    int k = 0;
    for (size_t i=0; i<points[1].size(); i++)
    {
        if (acceptTrackedPoint(i))
        {
            //initial[k] = initial[i];
			points[0][k] = points[0][i];
            points[1][k++] = points[1][i];
        }
    }
	points[0].resize(k);
    points[1].resize(k);
    //initial.resize(k);
    // 显示特征点和运动轨迹
  //  for (size_t i=0; i<points[1].size(); i++)
  //  {
  //      //line(output, initial[i], points[1][i], Scalar(0, 0, 255));
		//line(output, points[0][i], points[1][i], Scalar(255, 255, 0));
  //      circle(output, points[1][i], 3, Scalar(0, 255, 0), -1);
  //  }
	//cout << points[1].size() << endl;

    // 把当前跟踪结果作为下一此参考
    swap(points[1], points[0]);
    swap(gray_prev, gray);  
    //imshow(window_name, output);
	
	return drawRect(output);
}

//  检测新点是否应该被添加
// return: 是否被添加标志
bool addNewPoints()
{
	return true;
    //return points[0].size() <= 10;
}

//决定哪些跟踪点被接受
bool acceptTrackedPoint(int i)
{
	//return true;
	//return points[0][i].x < points[1][i].x;
    //return status[i] && ((abs(points[0][i].x - points[1][i].x) + abs(points[0][i].y - points[1][i].y)) > 2);
	//return status[i] && ((abs(points[0][i].x - points[1][i].x) + abs(points[0][i].y - points[1][i].y)) > 2) && points[0][i].x < points[1][i].x;
	return status[i] && ((abs(points[0][i].x - points[1][i].x) + abs(points[0][i].y - points[1][i].y)) > 2) && 
		((abs(points[0][i].x - points[1][i].x) + abs(points[0][i].y - points[1][i].y)) < 100);
}