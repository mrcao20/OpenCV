#include "stdafx.h"
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include "opencv2/imgproc.hpp"
#include <opencv2\ml.hpp>
#include <iostream>  
#include <string>  
#include <windows.h>
#include <cmath>
  
using namespace std;  
using namespace cv;  
using namespace ml;

Ptr<ANN_MLP> trainNeuralNetwork(vector<Point2f> data, vector<vector<float>> result);
Point2f find(Mat& img_object1, Mat& img_object2, Mat& img_scene);
void readFileToVector(vector<vector<float>> &result);
void start();

//int main()  
//{  
//	start();
//	return 0;  
//}  

void start()
{
	SYSTEMTIME start;
	GetLocalTime(&start);
	vector<Point2f> data;
	for(int i = 0; i <= 15; i++)
	{
		char temp[8];
		sprintf_s(temp, "%d.jpg", i);
		string str = temp;
		Mat img_scene = imread("pic\\temp\\" + str, CV_LOAD_IMAGE_GRAYSCALE);
		sprintf_s(temp, "%d.jpg", (i + 200));
		str = temp;
		Mat img_door = imread("pic\\temp\\" + str, CV_LOAD_IMAGE_GRAYSCALE);
		sprintf_s(temp, "%d.jpg", (i + 100));
		str = temp;
		Mat img_charactor = imread("pic\\temp\\" + str, CV_LOAD_IMAGE_GRAYSCALE);
		Point2f target = find(img_door, img_charactor, img_scene);
		/*if(abs(target.x) < 10.0)
			target.x = 0;
		if(abs(target.y) < 10.0)
			target.y = 0;*/
		data.push_back(target);
	}

	vector<vector<float>> result;
	readFileToVector(result);
	
	Ptr<ANN_MLP> ann = trainNeuralNetwork(data, result);
	Mat img_scene = imread("pic\\temp\\target.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	Mat img_charactor = imread("pic\\temp\\target1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	Mat img_door = imread("pic\\temp\\target2.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	//data.push_back(find(img_door, img_charactor, img_scene));
	Point2f target = find(img_door, img_charactor, img_scene);
	//cout << target << endl;
	/*if(abs(target.x) < 10.0)
		target.x = 0;
	if(abs(target.y) < 10.0)
		target.y = 0;*/
	Mat sampleMat = (Mat_<float>(1, 2) << (target.x + 800) / 1600.0, (target.y + 600) / 1200.0);  
	//Mat sampleMat = (Mat_<float>(1, 2) << data[2].x, data[2].y);  
    Mat responseMat;  
    ann->predict(sampleMat, responseMat);  
    float* p = responseMat.ptr<float>(0);  
	//cout << p[0] << " " << p[1] << " " << p[2] << " " << p[3] << endl;
	vector<string> direction;
	if(p[0] > 0)
	{
		direction.push_back("UP");
	}
	else if(p[1] > 0)
	{
		direction.push_back("RIGHT");
	}
	else if(p[2] > 0)
	{
		direction.push_back("DOWN");
	}
	else if(p[3] > 0)
	{
		direction.push_back("LEFT");
	}
	cout << "按下";
	for(int i = 0; i < direction.size(); i++)
	{
		cout << direction[i] << " ";
	}
	cout << endl;

	SYSTEMTIME end;
	GetLocalTime(&end);
	cout << (end.wMinute - start.wMinute) * 60 + (end.wSecond - start.wSecond) * 1000 + (end.wMilliseconds - start.wMilliseconds) << "ms" << endl;

}

Ptr<ANN_MLP> trainNeuralNetwork(vector<Point2f> data, vector<vector<float>> result)
{	

	int length = data.size();
	if(length != result.size())
	{
		cout << "训练数据长度不等" << endl;
		return Ptr<ANN_MLP>();
	}
	Mat trainingDataMat = Mat(length, 2, CV_32FC1);
	for(int i = 0; i < length; i++)
	{
		trainingDataMat.at<float>(i, 0) = (data[i].x + 800) / 1600.0;
		trainingDataMat.at<float>(i, 1) = (data[i].y + 600) / 1200.0;
		/*trainingDataMat.at<float>(i, 0) = data[i].x;
		trainingDataMat.at<float>(i, 1) = data[i].y;*/
	}
	cout << trainingDataMat << endl;
	Mat labelsMat = Mat(length, 4, CV_32FC1);
	for(int i = 0; i < length; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			labelsMat.at<float>(i, j) = result[i][j];
		}
	}
	cout << labelsMat << endl;
	Mat layerSizes = (Mat_<int>(1, 3) << 2, 10, 4); 

    Ptr<ANN_MLP> ann = ANN_MLP::create();  
    ann->setLayerSizes(layerSizes);//  
	ann->setActivationFunction(ANN_MLP::SIGMOID_SYM, 1, 1);  
//  ann->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 300, FLT_EPSILON));  
	ann->setTrainMethod(ANN_MLP::BACKPROP, 0.1, 0.9);  
    Ptr<TrainData> tData = TrainData::create(trainingDataMat, ROW_SAMPLE, labelsMat);  
	for(int i = 0; i < 100; i++)
	{
		ann->train(tData);
	}

	return ann;
}
