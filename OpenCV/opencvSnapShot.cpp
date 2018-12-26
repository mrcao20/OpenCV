#include "stdafx.h"
//#ifdef _MSC_VER  
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )  
//#endif  
//#define _AFXDLL//为了方便是用mfc类    
#include<afxwin.h>    
#include<opencv\cv.h>    
#include <opencv2\opencv.hpp>

void snapShot(cv::Mat& image){      
	//int nWidth = GetSystemMetrics(SM_CXSCREEN);//得到屏幕的分辨率的x    
    //int nHeight = GetSystemMetrics(SM_CYSCREEN);//得到屏幕分辨率的y    
	int nWidth = 800;
    int nHeight = 600;
	int x = 235;
	int y = 130;
    LPVOID screenCaptureData = new char[nWidth*nHeight*4];    
    memset(screenCaptureData,0,nWidth);    
    // Get desktop DC, create a compatible dc, create a comaptible bitmap and select into compatible dc.    
    HDC hDDC = GetDC( GetDesktopWindow() );//得到屏幕的dc    
    HDC hCDC = CreateCompatibleDC(hDDC);//    
    HBITMAP hBitmap =CreateCompatibleBitmap(hDDC,nWidth,nHeight);//得到位图    
    SelectObject(hCDC,hBitmap); //好像总得这么写。  
              
    BitBlt(hCDC,0,0,nWidth,nHeight,hDDC,x,y,SRCCOPY);    
    
    GetBitmapBits(hBitmap,nWidth*nHeight*4,screenCaptureData);//得到位图的数据，并存到screenCaptureData数组中。    
    IplImage *img = cvCreateImage(cvSize(nWidth,nHeight),8,4);//创建一个rgba格式的IplImage,内容为空    
    memcpy(img->imageData,screenCaptureData,nWidth*nHeight*4);//这样比较浪费时间，但写的方便。这里必须得是*4，上面的链接写的是*3，这是不对的。    
    IplImage *img2 = cvCreateImage(cvSize(nWidth,nHeight),8,3);//创建一个bgr格式的IplImage，可以没有这个Img2这个变量。    
    cvCvtColor(img,img2,CV_BGRA2BGR);   
	image = cv::cvarrToMat(img2, true);
	delete screenCaptureData;
	cvReleaseImage(&img);
	cvReleaseImage(&img2);
}    