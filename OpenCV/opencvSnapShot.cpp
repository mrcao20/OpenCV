#include "stdafx.h"
//#ifdef _MSC_VER  
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )  
//#endif  
//#define _AFXDLL//Ϊ�˷�������mfc��    
#include<afxwin.h>    
#include<opencv\cv.h>    
#include <opencv2\opencv.hpp>

void snapShot(cv::Mat& image){      
	//int nWidth = GetSystemMetrics(SM_CXSCREEN);//�õ���Ļ�ķֱ��ʵ�x    
    //int nHeight = GetSystemMetrics(SM_CYSCREEN);//�õ���Ļ�ֱ��ʵ�y    
	int nWidth = 800;
    int nHeight = 600;
	int x = 235;
	int y = 130;
    LPVOID screenCaptureData = new char[nWidth*nHeight*4];    
    memset(screenCaptureData,0,nWidth);    
    // Get desktop DC, create a compatible dc, create a comaptible bitmap and select into compatible dc.    
    HDC hDDC = GetDC( GetDesktopWindow() );//�õ���Ļ��dc    
    HDC hCDC = CreateCompatibleDC(hDDC);//    
    HBITMAP hBitmap =CreateCompatibleBitmap(hDDC,nWidth,nHeight);//�õ�λͼ    
    SelectObject(hCDC,hBitmap); //�����ܵ���ôд��  
              
    BitBlt(hCDC,0,0,nWidth,nHeight,hDDC,x,y,SRCCOPY);    
    
    GetBitmapBits(hBitmap,nWidth*nHeight*4,screenCaptureData);//�õ�λͼ�����ݣ����浽screenCaptureData�����С�    
    IplImage *img = cvCreateImage(cvSize(nWidth,nHeight),8,4);//����һ��rgba��ʽ��IplImage,����Ϊ��    
    memcpy(img->imageData,screenCaptureData,nWidth*nHeight*4);//�����Ƚ��˷�ʱ�䣬��д�ķ��㡣����������*4�����������д����*3�����ǲ��Եġ�    
    IplImage *img2 = cvCreateImage(cvSize(nWidth,nHeight),8,3);//����һ��bgr��ʽ��IplImage������û�����Img2���������    
    cvCvtColor(img,img2,CV_BGRA2BGR);   
	image = cv::cvarrToMat(img2, true);
	delete screenCaptureData;
	cvReleaseImage(&img);
	cvReleaseImage(&img2);
}    