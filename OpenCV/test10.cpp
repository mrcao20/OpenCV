#include "stdafx.h"  
#include <iostream>  
#include <opencv2/core/core.hpp>  
#include <opencv/cv.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <time.h>  
#include <math.h>  
#include <ctype.h>  
#include <stdio.h>  
#include <string.h>  
#include<windows.h>  
#include <mmsystem.h>  
  
//���ٲ���  
const double MHI_DURATION = 0.5;//������ʱ��  
const double MAX_TIME_DELTA = 0.5;  
const double MIN_TIME_DELTA = 0.05;  
const int N = 3;  
const int CONTOUR_MAX_AERA = 100;//�������  
  
IplImage **buf = 0;  
int last = 0;  
int flag;  
IplImage *mhi = 0; // MHI: motion history image  
CvConnectedComp *cur_comp, min_comp;  
CvConnectedComp comp;  
CvMemStorage *storage;  
CvPoint pt[4];  
// img �C ������Ƶ֡  
// dst �C �����  
void update_mhi(IplImage* img, IplImage* dst, int diff_threshold)  
{  
    double timestamp = clock() / 100.; //��ȡ��ǰʱ��  
    CvSize size = cvSize(img->width, img->height);  
    int i, idx1, idx2;  
    IplImage* silh;  
    IplImage* pyr = cvCreateImage(cvSize((size.width & -2) / 2, (size.height & -2) / 2), 8, 1);  
    CvMemStorage *stor;  
    CvSeq *cont;  
    if (!mhi || mhi->width != size.width || mhi->height != size.height)  
    {  
        if (buf == 0)  
        {  
            buf = (IplImage**)malloc(N*sizeof(buf[0]));//��̬�ڴ����  
            memset(buf, 0, N*sizeof(buf[0]));  
        }  
  
        for (i = 0; i < N; i++)  
        {  
            cvReleaseImage(&buf[i]);  
            buf[i] = cvCreateImage(size, IPL_DEPTH_8U, 1);  
            cvZero(buf[i]);  
        }  
        cvReleaseImage(&mhi);  
        mhi = cvCreateImage(size, IPL_DEPTH_32F, 1);  
        cvZero(mhi);  
    }  
    cvCvtColor(img, buf[last], CV_BGR2GRAY); //rgb->gray  
    idx1 = last;  
    idx2 = (last + 1) % N;  
    last = idx2;  
    // ��֡��  
    silh = buf[idx2];  
    cvAbsDiff(buf[idx1], buf[idx2], silh); //��֡����  
    // �Բ�ͼ������ֵ��  
    cvThreshold(silh, silh, 30, 255, CV_THRESH_BINARY); //src(x,y)>threshold ,dst(x,y) = max_value; ����,dst��x,y��=0;  
  
    cvUpdateMotionHistory(silh, mhi, timestamp, MHI_DURATION); //�������ص���˶���ʷ  
    cvCvtScale(mhi, dst, 255. / MHI_DURATION,  
        (MHI_DURATION - timestamp)*255. / MHI_DURATION);//timestamp��ʱ���;MHI_DURATION����õ��ǵ�ǰʱ��  
    cvCvtScale(mhi, dst, 255. / MHI_DURATION, 0);  
  
    // ��ֵ�˲�������С������  
    cvSmooth(dst, dst, CV_MEDIAN, 3, 0, 0, 0);  
  
    // ���²�����ȥ������  
    cvPyrDown(dst, pyr, 7);  
    cvDilate(pyr, pyr, 0, 1); // �����Ͳ���������Ŀ��Ĳ������ն�  
    cvPyrUp(pyr, dst, 7);  
    //  
    // ����ĳ���������ҵ�����  
    //  
    // Create dynamic structure and sequence.  
    stor = cvCreateMemStorage(0);  
    cont = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), stor);  
  
    // �ҵ���������  
    cvFindContours(dst, stor, &cont, sizeof(CvContour),  
        CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));  
    // ֱ��ʹ��CONTOUR�еľ�����������  
    for (; cont; cont = cont->h_next)  
    {  
        flag = 0;  
        CvRect r = ((CvContour*)cont)->rect;  
        if (r.height * r.width > CONTOUR_MAX_AERA)  
        {  
            cvRectangle(img, cvPoint(r.x, r.y),  
                cvPoint(r.x + r.width, r.y + r.height),  
                CV_RGB(255, 0, 0), 1, CV_AA, 0);  
            flag = 1;  
        }  
        else  
        {  
            flag = 0;  
        }  
    }  
    cvReleaseMemStorage(&stor);  
    cvReleaseImage(&pyr);  
}  
  
int main(int argc, char** argv)  
{  
    IplImage* motion = 0;  
    CvCapture* capture = 0; //��Ƶ��ȡ�ṹ   
      
    while (1){  
        capture = cvCreateCameraCapture(0);//������ͷ��Ƶ  
        //capture = cvCreateFileCapture("C:\\Users\\Lijunliang\\Desktop\\0.5X.avi");    //��������Ƶ�ļ�  
        if (capture)  
        {  
            cvNamedWindow("Motion", 1);  
            for (;;)  
            {  
                SYSTEMTIME sys;//��ȡ��ǰϵͳʱ��  
                GetLocalTime(&sys);  
                char* t_y = new char[128];  
                sprintf(t_y, ("%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d"),  
  
                    sys.wYear, sys.wMonth, sys.wDay,  
                    sys.wHour, sys.wMinute, sys.wSecond);  
  
                CvFont font;  
                cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 0.5, 0.5, 0, 2, 8);  
                IplImage* image;  
                if (!cvGrabFrame(capture)) //������ͷ������Ƶ�ļ���ץȡ֡  
                    break;  
                image = cvRetrieveFrame(capture); //ȡ���ɺ���cvGrabFrameץȡ��ͼ��,�����ɺ���cvGrabFrame ץȡ��ͼ���ָ��  
                if (image)  
                {  
                    if (!motion)  
                    {  
                        motion = cvCreateImage(cvSize(image->width, image->height), 8, 1);  
                        cvZero(motion);  
                        motion->origin = image->origin; ///* 0 - ������ṹ, 1 - �ס���ṹ (Windows bitmaps ���) */  
                    }  
                }  
                update_mhi(image, motion, 6);  
                cvPutText(image, t_y, cvPoint(10, 25), &font, CV_RGB(255, 0, 0));  
                cvShowImage("Motion", image);  
                if (cvWaitKey(10) >= 0)  
                    break;  
            }  
            cvReleaseCapture(&capture);  
            cvDestroyWindow("Motion");  
        }  
    }  
    return 0;  
} 