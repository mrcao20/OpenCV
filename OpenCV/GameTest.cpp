#include "stdafx.h"
#include <iostream>
#include <opencv2\opencv.hpp>
#include "VirtualKeyBoard.h"
#include "Keys.h"
#include <Windows.h>

using namespace cv;
using namespace std;

//map<string, int> keys;

Point2f find(Mat& img_character, Mat& img_scene);
Point2f find(Mat& img_door, Mat& img_character, Mat& img_scene);
void snapShot(cv::Mat& image);

//void init(){
//	keys.insert(pair<string, int>("Q", Keys::Q));
//	keys.insert(pair<string, int>("W", Keys::W));
//	keys.insert(pair<string, int>("E", Keys::E));
//	keys.insert(pair<string, int>("R", Keys::R));
//	keys.insert(pair<string, int>("T", Keys::T));
//	keys.insert(pair<string, int>("A", Keys::A));
//	keys.insert(pair<string, int>("S", Keys::S));
//	keys.insert(pair<string, int>("D", Keys::D));
//	keys.insert(pair<string, int>("F", Keys::F));
//	keys.insert(pair<string, int>("G", Keys::G));
//	keys.insert(pair<string, int>("H", Keys::H));
//	keys.insert(pair<string, int>("Z", Keys::Z));
//	keys.insert(pair<string, int>("X", Keys::X));
//	keys.insert(pair<string, int>("C", Keys::C));
//	keys.insert(pair<string, int>("V", Keys::V));
//	keys.insert(pair<string, int>("SPACE", Keys::SPACE));
//	keys.insert(pair<string, int>("ESC", Keys::ESC));
//	keys.insert(pair<string, int>("F10", Keys::F10));
//	keys.insert(pair<string, int>("F12", Keys::F12));
//	keys.insert(pair<string, int>("UP", Keys::UP));
//	keys.insert(pair<string, int>("LEFT", Keys::LEFT));
//	keys.insert(pair<string, int>("DOWN", Keys::DOWN));
//	keys.insert(pair<string, int>("RIGHT", Keys::RIGHT));
//	keys.insert(pair<string, int>("DELETE", Keys::_DELETE));
//}

//void main(){
//	DD_init();
//	Sleep(2000);
//	mousePress();
//}

bool isFindPoint(Point2f &point){
	if(point.x == -1 && point.y == -1){
		return false;
	}
	return true;
}
double light(IplImage * image);
Point2f judgeButton(string srcNum){
	Mat imgSrc = imread("pic\\target\\" + srcNum, CV_LOAD_IMAGE_GRAYSCALE);
	//Mat imgScene = imread("E:\\screenshots\\gameTest\\0.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	Mat imgScene;
	snapShot(imgScene);
	IplImage temp(imgScene);
	double a = light(&temp);
	if(a < -118.0){
		return Point2f(-1, -1);
	}
	//imgScene = Mat(imgScene, Rect(235, 130, 800, 600));
	if(!imgSrc.data || !imgScene.data){
		cout << "Í¼Æ¬¶ÁÈëÊ§°Ü" << endl;
		return Point2f(-1, -1);
	}
	Point2f imgCharacterPoint = find(imgSrc, imgScene);
	imgSrc.release();
	imgScene.release();
	if(!isFindPoint(imgCharacterPoint)){
		return Point2f(-1, -1);
	}
	return imgCharacterPoint;
}

Point2f judgeButton(string srcNum, string doorNum){
	Mat imgSrc = imread("pic\\target\\" + srcNum, CV_LOAD_IMAGE_GRAYSCALE);
	Mat imgDoor = imread("pic\\target\\" + doorNum, CV_LOAD_IMAGE_GRAYSCALE);
	//Mat imgScene = imread("E:\\screenshots\\gameTest\\0.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	Mat imgScene;
	snapShot(imgScene);
	//imgScene = Mat(imgScene, Rect(235, 130, 800, 600));
	if(!imgSrc.data || !imgScene.data || !imgDoor.data){
		cout << "Í¼Æ¬¶ÁÈëÊ§°Ü" << endl;
		return Point2f(-1, -1);
	}
	Point2f imgCharacterPoint = find(imgDoor, imgSrc, imgScene);
	imgSrc.release();
	imgDoor.release();
	imgScene.release();
	if(!isFindPoint(imgCharacterPoint)){
		return Point2f(-1, -1);
	}
	return imgCharacterPoint;
}