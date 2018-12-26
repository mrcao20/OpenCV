#include "stdafx.h"
#include"Homography.h"

//int main()
//{
//    string imgPath1 = "E:\\screenshots\\tempNULL\\14.jpg";
//    string imgPath2 = "E:\\screenshots\\tempNULL\\13.jpg";
//    Mat img1 = imread(imgPath1, CV_LOAD_IMAGE_GRAYSCALE);
//    Mat img2 = imread(imgPath2, CV_LOAD_IMAGE_GRAYSCALE);
//
//    Homography homo12(img1,img2);
//    Mat h12 = homo12.getHomography();
//
//    Mat h21;
//    invert(h12, h21, DECOMP_LU);
//
//    Mat canvas;
//    Mat img1_color = imread(imgPath1, CV_LOAD_IMAGE_COLOR);
//    Mat img2_color = imread(imgPath2, CV_LOAD_IMAGE_COLOR); 
//
//    warpPerspective(img2_color, canvas, h21, Size(img1.cols*2, img1.rows));
//    img1_color.copyTo(canvas(Range::all(), Range(0, img1.cols)));
//	//imwrite("1.jpg", canvas);
//    imshow("canvas",canvas);
//
//    waitKey(0);
//    return 0;
//}