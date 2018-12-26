#include "stdafx.h"
#include <opencv2\opencv.hpp>
# include "opencv2/core/core.hpp"
# include "opencv2/features2d/features2d.hpp"
# include "opencv2/highgui/highgui.hpp"
# include "opencv2/imgproc/imgproc.hpp"
#include <opencv2\xfeatures2d.hpp>
#include"opencv2/calib3d/calib3d.hpp"
#include<iostream>
#include <opencv2/stitching.hpp>
using namespace cv;
using namespace std;
using namespace xfeatures2d;

void OptimizeSeam(Mat& img1, Mat& trans, Mat& dst);
void OptimizeSeam1(Mat& img1, Mat& trans, Mat& dst);
Mat stitcher(Mat image02, Mat image01);
void upDownStitcher(Mat &image01, Mat &image02);
void OptimizeSeamFlip(Mat& img1, Mat& trans, Mat& dst);

typedef struct
{
    Point2f left_top;
    Point2f left_bottom;
    Point2f right_top;
    Point2f right_bottom;
}four_corners_t;
typedef std::set<std::pair<int,int> > MatchesSet;
four_corners_t corners;

void CalcCorners(const Mat& H, const Mat& src)
{
    double v2[] = { 0, 0, 1 };//���Ͻ�
    double v1[3];//�任�������ֵ
    Mat V2 = Mat(3, 1, CV_64FC1, v2);  //������
    Mat V1 = Mat(3, 1, CV_64FC1, v1);  //������
    V1 = H * V2;

    //���Ͻ�(0,0,1)
    std::cout << "V2: " << V2 << endl;
    std::cout << "V1: " << V1 << endl;
    corners.left_top.x = v1[0] / v1[2];
    corners.left_top.y = v1[1] / v1[2];

    //���½�(0,src.rows,1)
    v2[0] = 0;
    v2[1] = src.rows;
    v2[2] = 1;
    V2 = Mat(3, 1, CV_64FC1, v2);  //������
    V1 = Mat(3, 1, CV_64FC1, v1);  //������
    V1 = H * V2;
    corners.left_bottom.x = v1[0] / v1[2];
    corners.left_bottom.y = v1[1] / v1[2];

    //���Ͻ�(src.cols,0,1)
    v2[0] = src.cols;
    v2[1] = 0;
    v2[2] = 1;
    V2 = Mat(3, 1, CV_64FC1, v2);  //������
    V1 = Mat(3, 1, CV_64FC1, v1);  //������
    V1 = H * V2;
    corners.right_top.x = v1[0] / v1[2];
    corners.right_top.y = v1[1] / v1[2];
	
    //���½�(src.cols,src.rows,1)
    v2[0] = src.cols;
    v2[1] = src.rows;
    v2[2] = 1;
    V2 = Mat(3, 1, CV_64FC1, v2);  //������
    V1 = Mat(3, 1, CV_64FC1, v1);  //������
    V1 = H * V2;
    corners.right_bottom.x = v1[0] / v1[2];
    corners.right_bottom.y = v1[1] / v1[2];

}

void CalcCorners1(const Mat& H, const Mat& src)
{
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( src.cols, 0 );
	obj_corners[2] = cvPoint( src.cols, src.rows ); obj_corners[3] = cvPoint( 0, src.rows );
	std::vector<Point2f> scene_corners(4);
  
	perspectiveTransform( obj_corners, scene_corners, H);
	corners.left_top = scene_corners[0];
	corners.right_top = scene_corners[1];
	corners.right_bottom = scene_corners[2];
	corners.left_bottom = scene_corners[3];

}

//int main(int argc, char *argv[])
//{
//	//Mat image01 = imread("E:\\screenshots\\target\\TheSecondScene\\theThirdScene.jpg", 1);    //��ͼ
//	////image01 = Mat(image01, Rect(0, 206, 800, 313));
//	////flip(image01, image01,-1);
//	////imshow("dst", image01);
// //   //cv::waitKey();
//	////string str;
//	////for(int i = 77; i <= 83; i++){
//	////	stringstream strStream;
//	////	strStream << i;
//	////	strStream >> str;
//	////	Mat image02 = imread("E:\\screenshots\\target\\" + str + ".jpg", 1);    //��ͼ
//	////	image02 = Mat(image02, Rect(0, 206, 800, 313));
//	////	//flip(image02, image02,-1);
//	////	image01 = stitcher(image01, image02);
//	////}
//	//Mat image02 = imread("E:\\screenshots\\target\\TheSecondScene\\theThirdScene1.jpg", 1);    //��ͼ
//	////image02 = Mat(image02, Rect(0, 206, 800, 313));
//	////flip(image02, image02,-1);
//	//image01 = stitcher(image01, image02);
//
//	Mat image01 = imread("E:\\screenshots\\target\\TheSecondScene\\theScene2.jpg");
//	Mat image02 = imread("E:\\screenshots\\temp\\34.jpg");
//	image02 = Mat(image02, Rect(0, 206, 795, 308));
//	upDownStitcher(image01, image02);
//	/*image02 = imread("E:\\screenshots\\target\\TheFourthScene\\theFirstScene.jpg");
//	upDownStitcher(image01, image02);*/
//	//imshow("p1", image01);
//    //imshow("p2", image02);
//	//waitKey(0);
//	//flip(image01, image01,-1);
//	imshow("dst", image01);
//    cv::waitKey();
//	//imwrite("E:\\screenshots\\target\\TheSecondScene\\theScene2.jpg", image01);
//    return 0;
//}

Mat imageFusion(Mat &frame){
	Mat bgImage = imread("E:\\screenshots\\target\\TheSecondScene\\theScene2.jpg");
	/*cvtColor(frame, frame, COLOR_BGR2GRAY);
	cvtColor(bgImage, bgImage, COLOR_BGR2GRAY);*/
	return stitcher(bgImage, frame);
}

void upDownStitcher(Mat &image01, Mat &image02){
	if(image01.cols > image02.cols){
		image01 = Mat(image01, Rect(0,0,image02.cols, image01.rows));
	}else if(image01.cols < image02.cols){
		image02 = Mat(image02, Rect(0,0,image01.cols, image02.rows));
	}
	transpose(image01, image01);
	transpose(image02, image02);
	flip(image01, image01,1);
	flip(image02, image02,1);
	image01 = stitcher(image01, image02);
	transpose(image01, image01);
	flip(image01, image01,0);
}

Mat stitcher(Mat image02, Mat image01){
	
	/*transpose(image02, image02);
	transpose(image01, image01);
	flip(image01, image01,0);
	flip(image02, image02,0);*/
    /*imshow("p2", image01);
    imshow("p1", image02);
	waitKey(0);*/
    //�Ҷ�ͼת��  
    Mat image1, image2;
    cvtColor(image01, image1, CV_RGB2GRAY);
    cvtColor(image02, image2, CV_RGB2GRAY);


	//��ȡ������    
	Ptr<FeatureDetector> Detector = SURF::create(1000);    // ����������ֵ��������������ȣ�ֵԽ���Խ�٣�Խ��׼ 
	vector<KeyPoint> keyPoint1, keyPoint2;
	Detector->detect(image1, keyPoint1);
	Detector->detect(image2, keyPoint2);

	//������������Ϊ�±ߵ�������ƥ����׼��    
	Ptr<FeatureDetector> Descriptor = SURF::create();  
	Mat imageDesc1, imageDesc2;
	Descriptor->compute(image1, keyPoint1, imageDesc1);
	Descriptor->compute(image2, keyPoint2, imageDesc2);

	FlannBasedMatcher matcher;
	vector<vector<DMatch> > matchePoints;
	vector<DMatch> GoodMatchePoints;
	
	MatchesSet matches;

	vector<Mat> train_desc(1, imageDesc1);
	matcher.add(train_desc);
	matcher.train();

	matcher.knnMatch(imageDesc2, matchePoints, 2);
	
	// Lowe's algorithm,��ȡ����ƥ���
	for (int i = 0; i < matchePoints.size(); i++)
	{
		if (matchePoints[i][0].distance < 0.4 * matchePoints[i][1].distance)
		{
			GoodMatchePoints.push_back(matchePoints[i][0]);
			matches.insert(make_pair(matchePoints[i][0].queryIdx, matchePoints[i][0].trainIdx));
		}
	}
	cout<<"\n1->2 matches: " << GoodMatchePoints.size() << endl;


	FlannBasedMatcher matcher2;
	matchePoints.clear();
	vector<Mat> train_desc2(1, imageDesc2);
	matcher2.add(train_desc2);
	matcher2.train();

	matcher2.knnMatch(imageDesc1, matchePoints, 2);
	// Lowe's algorithm,��ȡ����ƥ���
	for (int i = 0; i < matchePoints.size(); i++)
	{
		if (matchePoints[i][0].distance < 0.4 * matchePoints[i][1].distance)
		{
			if (matches.find(make_pair(matchePoints[i][0].trainIdx, matchePoints[i][0].queryIdx)) == matches.end())
			{
				GoodMatchePoints.push_back(DMatch(matchePoints[i][0].trainIdx, matchePoints[i][0].queryIdx, matchePoints[i][0].distance));
			}
        
		}
	}
	cout<<"1->2 & 2->1 matches: " << GoodMatchePoints.size() << endl;

    //Mat first_match;
    //drawMatches(image02, keyPoint2, image01, keyPoint1, GoodMatchePoints, first_match);
	//cout << "total match points: " << GoodMatchePoints.size() << endl;
    //imshow("first_match ", first_match);
	//waitKey(0);
    vector<Point2f> imagePoints1, imagePoints2;

    for (int i = 0; i<GoodMatchePoints.size(); i++)
    {
        imagePoints2.push_back(keyPoint2[GoodMatchePoints[i].queryIdx].pt);
        imagePoints1.push_back(keyPoint1[GoodMatchePoints[i].trainIdx].pt);
    }



    //��ȡͼ��1��ͼ��2��ͶӰӳ����� �ߴ�Ϊ3*3  
    Mat homo = findHomography(imagePoints1, imagePoints2, CV_RANSAC);
    ////Ҳ����ʹ��getPerspectiveTransform�������͸�ӱ任���󣬲���Ҫ��ֻ����4���㣬Ч���Բ�  
    //Mat   homo=getPerspectiveTransform(imagePoints1,imagePoints2);  
    cout << "�任����Ϊ��\n" << homo << endl << endl; //���ӳ�����      

   //������׼ͼ���ĸ���������
    CalcCorners1(homo, image01);
    cout << "left_top:" << corners.left_top << endl;
    cout << "left_bottom:" << corners.left_bottom << endl;
    cout << "right_top:" << corners.right_top << endl;
    cout << "right_bottom:" << corners.right_bottom << endl;

    //ͼ����׼  
	Mat imageTransform1;
    warpPerspective(image01, imageTransform1, homo, Size(MAX(corners.right_top.x, corners.right_bottom.x), image02.rows));
    //warpPerspective(image01, imageTransform2, adjustMat*homo, Size(image02.cols*1.3, image02.rows*1.8));
    //imshow("ֱ�Ӿ���͸�Ӿ���任", imageTransform1);
    //imwrite("trans1.jpg", imageTransform1);
	/*cvtColor(image02, image02, COLOR_BGR2GRAY);
	cvtColor(image01, image01, COLOR_BGR2GRAY);*/
	Mat dst(image02.rows, image02.cols, image02.type());
	dst.setTo(0);

	int x = corners.left_top.x >= 0 ? corners.left_top.x : 0;
	int y = corners.left_top.y >= 0 ? corners.left_top.y : 0;
	int width = (corners.right_top.x < dst.cols ? image01.cols : dst.cols - x) - 2;
	int height = (corners.left_bottom.y < dst.rows ? image01.rows : dst.rows - y) - 2;
	image01 = Mat(image01, Rect(0, 0, width, height));
	image02.copyTo(dst);
	image01.copyTo(dst(Rect(x, y, width, height)));
	cvtColor(image02, image02, COLOR_BGR2HLS);
	cvtColor(dst, dst, COLOR_BGR2HLS);
	vector<Mat> mvBgImage;
	vector<Mat> mvDst;
	split(image02, mvBgImage);
	split(dst, mvDst);
	mvBgImage[2].copyTo(mvDst[2]);
	merge(mvDst,dst);
	cvtColor(dst, dst, COLOR_HLS2BGR);

	/*int x = corners.left_top.x >= 0 ? corners.left_top.x : 0;
	int y = corners.left_top.y >= 0 ? corners.left_top.y : 0;
	int width = (corners.right_top.x < dst.cols ? image01.cols : dst.cols - x) - 2;
	int height = (corners.left_bottom.y < dst.rows ? image01.rows : dst.rows - y) - 2;
	image01 = Mat(image01, Rect(0, 0, width, height));
	cvtColor(image02, image02, COLOR_BGR2HLS);
	cvtColor(image01, image01, COLOR_BGR2HLS);
	vector<Mat> mvFrame;
	vector<Mat> mvBgImage;
	vector<Mat> mvDst;
	split(image01, mvFrame);
	split(image02, mvBgImage);
	split(dst, mvDst);
	mvBgImage[0].copyTo(mvDst[0]);
	mvBgImage[2].copyTo(mvDst[2]);
	mvBgImage[1].copyTo(mvDst[1]);
	mvFrame[0].copyTo(mvDst[0](Rect(x, y, width, height)));
	mvFrame[2].copyTo(mvDst[2](Rect(x, y, width, height)));
	merge(mvDst,dst);
	cvtColor(dst, dst, COLOR_HLS2BGR);*/

	/*image02.copyTo(dst);
	int x = corners.left_top.x >= 0 ? corners.left_top.x : 0;
	int y = corners.left_top.y >= 0 ? corners.left_top.y : 0;
	int width = (corners.right_top.x < dst.cols ? image01.cols : dst.cols - x) - 2;
	int height = (corners.left_bottom.y < dst.rows ? image01.rows : dst.rows - y) - 2;
	image01 = Mat(image01, Rect(0, 0, width, height));
	image01.copyTo(dst(Rect(x, y, width, height)));*/
	//cvtColor(dst, dst, COLOR_GRAY2BGR);
	imshow("b_dst", dst);
	waitKey();
	//image01.copyTo(dst(Rect(abs(corners.left_top.x), abs(corners.left_top.y), image02.cols - abs(corners.left_top.x), image02.rows - abs(corners.left_top.y))));

    //����ƴ�Ӻ��ͼ,����ǰ����ͼ�Ĵ�С
 //   int dst_width = imageTransform1.cols;  //ȡ���ҵ�ĳ���Ϊƴ��ͼ�ĳ���
 //   int dst_height = image02.rows;

 //   Mat dst(dst_height, dst_width, CV_8UC3);
 //   dst.setTo(0);
	//image01.copyTo(imageTransform1(Rect(imageTransform1.cols - image01.cols, 0, image01.cols, image01.rows)));
 //   imageTransform1.copyTo(dst(Rect(0, 0, imageTransform1.cols, imageTransform1.rows)));
	////image01.copyTo(dst(Rect(imageTransform1.cols - image01.cols, 0, image01.cols, image01.rows)));
 //   image02.copyTo(dst(Rect(0, 0, image02.cols, image02.rows)));

    //imshow("b_dst", dst);

	//dst.setTo(0);

    //OptimizeSeam(image02, imageTransform1, dst);
	//OptimizeSeam1(image02, imageTransform1, dst);
	//OptimizeSeamFlip(image02, imageTransform1, dst);

    /*imshow("dst", dst);
    waitKey();*/
	return dst;
}

void OptimizeSeamFlip(Mat& img1, Mat& trans, Mat& dst)
{
	int rows = dst.rows;
	int cols = dst.cols;
	int length = 0;
	uchar* p = img1.ptr<uchar>(0);  //��ȡ��i�е��׵�ַ
    uchar* t = trans.ptr<uchar>(0);
    uchar* d = dst.ptr<uchar>(0);
    for (int j = 0; j < cols; j++)
    {
		//�������ͼ��trans�������صĺڵ㣬����ȫ����img1�е�����
		if (t[j * 3] == 0 && t[j * 3 + 1] == 0 && t[j * 3 + 2] == 0)
		{
			length++;
		}
	}
	int width = 164 + length;
    for (int i = 0; i < rows; i++)
    {
        uchar* p = img1.ptr<uchar>(i);  //��ȡ��i�е��׵�ַ
        uchar* t = trans.ptr<uchar>(i);
        uchar* d = dst.ptr<uchar>(i);
        for (int j = 0; j < cols; j++)
        {
			if(j <= width){
				d[j * 3] = p[j * 3];
				d[j * 3 + 1] = p[j * 3 + 1];
				d[j * 3 + 2] = p[j * 3 + 2];
			}else{
				d[j * 3] = t[j * 3];
				d[j * 3 + 1] = t[j * 3 + 1];
				d[j * 3 + 2] = t[j * 3 + 2];
			}

        }
    }

}


void OptimizeSeam1(Mat& img1, Mat& trans, Mat& dst)
{
    int start = MIN(corners.left_top.x, corners.left_bottom.x);//��ʼλ�ã����ص��������߽�  
	start += 10;
    double processWidth = img1.cols - start;//�ص�����Ŀ��  
    int rows = dst.rows;
    int cols = img1.cols; //ע�⣬������*ͨ����
    double alpha = 1;//img1�����ص�Ȩ��  
    for (int i = 0; i < rows; i++)
    {
        uchar* p = img1.ptr<uchar>(i);  //��ȡ��i�е��׵�ַ
        uchar* t = trans.ptr<uchar>(i);
        uchar* d = dst.ptr<uchar>(i);
        for (int j = start; j < cols; j++)
        {
			//if(j < start + 50){
			//	//�������ͼ��trans�������صĺڵ㣬����ȫ����img1�е�����
			//	if (t[j * 3] == 0 && t[j * 3 + 1] == 0 && t[j * 3 + 2] == 0)
			//	{
			//	    alpha = 1;
			//	}
			//	else
			//	{
			//	    //img1�����ص�Ȩ�أ��뵱ǰ�������ص�������߽�ľ�������ȣ�ʵ��֤�������ַ���ȷʵ��  
			//	    alpha = (processWidth - (j - start)) / processWidth;
			//	}

			//	d[j * 3] = p[j * 3] * alpha + t[j * 3] * (1 - alpha);
			//	d[j * 3 + 1] = p[j * 3 + 1] * alpha + t[j * 3 + 1] * (1 - alpha);
			//	d[j * 3 + 2] = p[j * 3 + 2] * alpha + t[j * 3 + 2] * (1 - alpha);
			//}else{

			//	d[j * 3] = t[j * 3];
			//	d[j * 3 + 1] = t[j * 3 + 1];
			//	d[j * 3 + 2] = t[j * 3 + 2];
			//}
			d[j * 3] = t[j * 3];
			d[j * 3 + 1] = t[j * 3 + 1];
			d[j * 3 + 2] = t[j * 3 + 2];

        }
    }

}

//�Ż���ͼ�����Ӵ���ʹ��ƴ����Ȼ
void OptimizeSeam(Mat& img1, Mat& trans, Mat& dst)
{
    int start = MIN(corners.left_top.x, corners.left_bottom.x);//��ʼλ�ã����ص��������߽�  

    double processWidth = img1.cols - start;//�ص�����Ŀ��  
    int rows = dst.rows;
    int cols = img1.cols; //ע�⣬������*ͨ����
    double alpha = 1;//img1�����ص�Ȩ��  
    for (int i = 0; i < rows; i++)
    {
        uchar* p = img1.ptr<uchar>(i);  //��ȡ��i�е��׵�ַ
        uchar* t = trans.ptr<uchar>(i);
        uchar* d = dst.ptr<uchar>(i);
        for (int j = start; j < cols; j++)
        {
            //�������ͼ��trans�������صĺڵ㣬����ȫ����img1�е�����
            if (t[j * 3] == 0 && t[j * 3 + 1] == 0 && t[j * 3 + 2] == 0)
            {
                alpha = 1;
            }
            else
            {
                //img1�����ص�Ȩ�أ��뵱ǰ�������ص�������߽�ľ�������ȣ�ʵ��֤�������ַ���ȷʵ��  
                alpha = (processWidth - (j - start)) / processWidth;
            }

            d[j * 3] = p[j * 3] * alpha + t[j * 3] * (1 - alpha);
            d[j * 3 + 1] = p[j * 3 + 1] * alpha + t[j * 3 + 1] * (1 - alpha);
            d[j * 3 + 2] = p[j * 3 + 2] * alpha + t[j * 3 + 2] * (1 - alpha);

        }
    }

}