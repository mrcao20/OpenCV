#include "stdafx.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/stitching.hpp>
using namespace std;
using namespace cv;

void pictureStitcher();
//int main(int argc, char * argv[])
//{
//	/*transpose(img1, img1);
//	transpose(img2, img2);
//	flip(img1, img1,0);
//	flip(img2, img2,0);*/
//
//	/*transpose(pano, pano);
//	flip(pano, pano,1);*/
//	pictureStitcher();
//    
//}

void pictureStitcher(){
	vector<Mat> imgs;
	string str;
	/*for(int i = 3; i <= 4; i++){
		stringstream strStream;
		strStream << i;
		strStream >> str;
		Mat src = imread("E:\\screenshots\\target\\" + str + ".jpg");
		if (src.empty())
		{
			cout << "Can't read image" << endl;
			return;
		}
		src = Mat(src, Rect(0, 206, 800, 313));
		imgs.push_back(src);
	}*/
	Mat img1 = imread("E:\\screenshots\\target\\TheThirdScene\\theFirstScene.jpg");
	Mat img2 = imread("E:\\screenshots\\target\\TheThirdScene\\theSecondScene.jpg");
	Mat img3 = imread("E:\\screenshots\\target\\TheThirdScene\\theThirdScene.jpg");
	//img2 = Mat(img2, Rect(0, 206, 795, 308));
	//img1 = Mat(img1, Rect(0, 206, 800, 313));
	//img2 = Mat(img2, Rect(0, 206, 800, 313));
	/*imshow("全景图像", img1);
	waitKey(0);*/
	imgs.push_back(img1);
	imgs.push_back(img2);
	Stitcher stitcher = Stitcher::createDefault(false);
	
    // 使用stitch函数进行拼接
    Mat pano;
    Stitcher::Status status = stitcher.stitch(imgs, pano);
    if (status != Stitcher::OK)
    {
        cout << "Can't stitch images, error code = " << int(status) << endl;
        return;
    }
	imshow("全景图像", pano);

    if (waitKey() == 27)
        return;
}