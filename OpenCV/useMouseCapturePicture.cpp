#include "stdafx.h"
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2\imgproc.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>

using namespace std;
using namespace cv;  
  
cv::Mat org,dst,img,tmp;  
int i;

void on_mouse(int event,int x,int y,int flags,void *ustc)//event����¼����ţ�x,y������꣬flags��ק�ͼ��̲����Ĵ���  
{  
    static Point pre_pt = (-1,-1);//��ʼ����  
    static Point cur_pt = (-1,-1);//ʵʱ����  
    char temp[16];  
	//String temp;
    if (event == CV_EVENT_LBUTTONDOWN)//������£���ȡ��ʼ���꣬����ͼ���ϸõ㴦��Բ  
    {  
        org.copyTo(img);//��ԭʼͼƬ���Ƶ�img��  
        sprintf_s(temp,"(%d,%d)",x,y);  
		printf("%s", temp);
        pre_pt = Point(x,y);  
        putText(img,temp,pre_pt,FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,0,255),1,8);//�ڴ�������ʾ����  
        circle(img,pre_pt,2,Scalar(255,0,0,0),CV_FILLED,CV_AA,0);//��Բ  
        imshow("img",img);  
    }  
    else if (event == CV_EVENT_MOUSEMOVE && !(flags & CV_EVENT_FLAG_LBUTTON))//���û�а��µ����������ƶ��Ĵ�����  
    {  
        img.copyTo(tmp);//��img���Ƶ���ʱͼ��tmp�ϣ�������ʾʵʱ����  
        sprintf_s(temp,"(%d,%d)",x,y);  
        cur_pt = Point(x,y);  
        putText(tmp,temp,cur_pt,FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,0,255));//ֻ��ʵʱ��ʾ����ƶ�������  
        imshow("img",tmp);  
    }  
    else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))//�������ʱ������ƶ�������ͼ���ϻ�����  
    {  
        img.copyTo(tmp);  
        sprintf_s(temp,"(%d,%d)",x,y); 
        cur_pt = Point(x,y);  
        putText(tmp,temp,cur_pt,FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,0,255));  
        rectangle(tmp,pre_pt,cur_pt,Scalar(0,255,0,0),1,8,0);//����ʱͼ����ʵʱ��ʾ����϶�ʱ�γɵľ���  
        imshow("img",tmp);  
    }  
    else if (event == CV_EVENT_LBUTTONUP)//����ɿ�������ͼ���ϻ�����  
    {  
        org.copyTo(img);  
        sprintf_s(temp,"(%d,%d)",x,y);  
        cur_pt = Point(x,y);  
        putText(img,temp,cur_pt,FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,0,255));  
        circle(img,pre_pt,2,Scalar(255,0,0,0),CV_FILLED,CV_AA,0);  
        rectangle(img,pre_pt,cur_pt,Scalar(0,255,0,0),1,8,0);//���ݳ�ʼ��ͽ����㣬�����λ���img��  
        imshow("img",img);  
        img.copyTo(tmp);  
        //��ȡ���ΰ�Χ��ͼ�񣬲����浽dst��  
        int width = abs(pre_pt.x - cur_pt.x);  
        int height = abs(pre_pt.y - cur_pt.y);  
        if (width == 0 || height == 0)  
        {  
            printf("width == 0 || height == 0\n");  
            return;  
        }  
        dst = org(Rect(min(cur_pt.x,pre_pt.x),min(cur_pt.y,pre_pt.y),width,height));  
        namedWindow("dst");  
        imshow("dst",dst);  
		
        waitKey(0);  
		/*string fileName;
		cout << "input file name: ";
		cin >> fileName;
		imwrite("pic\\target\\" + fileName + ".jpg", dst);*/
		/*char temp[8];
		sprintf_s(temp, "%d.jpg", i);
		string str = temp;
		imwrite("E:\\Python\\AI_Brain\\tensorflow\\images\\" + str, dst);*/
		//imwrite("E:\\Python\\temp\\images\\1.jpg", dst);
    }  
}  

//void main()  
//{  
//	//org = imread("E:\\Python\\temp\\images\\1.jpg");  
//	//org.copyTo(img);  
//	//org.copyTo(tmp);  
//	//namedWindow("img");//����һ��img����  
//	//setMouseCallback("img",on_mouse,0);//���ûص�����  
//	//imshow("img",tmp);  
//	//cv::waitKey(0);  
//	for(i = 1; i <= 38; i++)
//	{
//		char temp[8];
//		sprintf_s(temp, "%d.jpg", i);
//		string str = temp;
//		org = imread("E:\\screenshots\\1\\" + str);  
//		org.copyTo(img);  
//		org.copyTo(tmp);  
//		namedWindow("img");//����һ��img����  
//		setMouseCallback("img",on_mouse,0);//���ûص�����  
//		imshow("img",tmp);  
//		cv::waitKey(0);  
//	}
//	
//}  