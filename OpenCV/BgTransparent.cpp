#include "stdafx.h"
#include<iostream>
#include<opencv.hpp>
using namespace cv;
using namespace std;
Mat createAlpha(cv::Mat& src)
{
	cv::Mat alpha = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
	cv::Mat gray = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
	cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			uchar temp = gray.at<uchar>(i, j);
			alpha.at<uchar>(i, j) = (temp == 255) ? 0 : 255;
		}
	}
	return alpha;
}
int addAlpha(cv::Mat& src, cv::Mat& dst, cv::Mat& alpha)
{
	if (src.channels() == 4)
	{
		return -1;
	}
	else if (src.channels() == 1)
	{
		cv::cvtColor(src, src, cv::COLOR_GRAY2RGB);
	}
	dst = Mat(src.rows, src.cols, CV_8UC4);
	std::vector<cv::Mat> srcChannels;
	std::vector<cv::Mat> dstChannels;
	//分离通道
	cv::split(src, srcChannels);
	dstChannels.push_back(srcChannels[0]);
	dstChannels.push_back(srcChannels[1]);
	dstChannels.push_back(srcChannels[2]);
	//添加透明度通道
	dstChannels.push_back(alpha);
	//合并通道
	cv::merge(dstChannels, dst);
	return 0;
}

Mat complete(cv::Mat& src, cv::Mat& dst) {
	cv::Mat gray = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
	cv::cvtColor(src, gray, cv::COLOR_RGBA2GRAY);
	std::vector<cv::Mat> srcChannels;
	std::vector<cv::Mat> dstChannels;
	cv::split(src, srcChannels);
	dstChannels.push_back(srcChannels[0]);
	dstChannels.push_back(srcChannels[1]);
	dstChannels.push_back(srcChannels[2]);
	Mat alpha = srcChannels[3];
	cv::Mat newAlpha = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			uchar temp = alpha.at<uchar>(i, j);
			if (temp == 255) {
				if (alpha.at<uchar>(i, j - 1) == 0 || alpha.at<uchar>(i, j + 1) == 0 || alpha.at<uchar>(i - 1, j) == 0 || alpha.at<uchar>(i + 1, j) == 0) {
					if (gray.at<uchar>(i, j) >= 200) {
						temp = 0;
					}
				}
			}
			newAlpha.at<uchar>(i, j) = temp;
		}
	}
	GaussianBlur(newAlpha, newAlpha, Size(5, 5), 5, 5);
	for (int i = 0; i < newAlpha.rows; i++)
	{
		uchar* p = newAlpha.ptr<uchar>(i);
		for (int j = 0; j < newAlpha.cols; j++)
		{
			if (p[j] > 150)
				p[j] = 255;
		}
	}
	dstChannels.push_back(newAlpha);
	Mat dst1 = Mat(src.rows, src.cols, CV_8UC4);
	cv::merge(dstChannels, dst1);
	swap(dst1, dst);
	return newAlpha;
}

//去除二值图像边缘的突出部
//uthreshold、vthreshold分别表示突出部的宽度阈值和高度阈值
//type代表突出部的颜色，0表示黑色，1代表白色 
void delete_jut(Mat& src, Mat& dst, int uthreshold, int vthreshold, int type)
{
	int threshold;
	src.copyTo(dst);
	int height = dst.rows;
	int width = dst.cols;
	int k;  //用于循环计数传递到外部
	for (int i = 0; i < height - 1; i++)
	{
		uchar* p = dst.ptr<uchar>(i);
		for (int j = 0; j < width - 1; j++)
		{
			if (type == 0)
			{
				//行消除
				if (p[j] == 255 && p[j + 1] == 0)
				{
					if (j + uthreshold >= width)
					{
						for (int k = j + 1; k < width; k++)
							p[k] = 255;
					}
					else
					{
						for (k = j + 2; k <= j + uthreshold; k++)
						{
							if (p[k] == 255) break;
						}
						if (p[k] == 255)
						{
							for (int h = j + 1; h < k; h++)
								p[h] = 255;
						}
					}
				}
				//列消除
				if (p[j] == 255 && p[j + width] == 0)
				{
					if (i + vthreshold >= height)
					{
						for (k = j + width; k < j + (height - i)*width; k += width)
							p[k] = 255;
					}
					else
					{
						for (k = j + 2 * width; k <= j + vthreshold * width; k += width)
						{
							if (p[k] == 255) break;
						}
						if (p[k] == 255)
						{
							for (int h = j + width; h < k; h += width)
								p[h] = 255;
						}
					}
				}
			}
			else  //type = 1
			{
				//行消除
				if (p[j] == 0 && p[j + 1] == 255)
				{
					if (j + uthreshold >= width)
					{
						for (int k = j + 1; k < width; k++)
							p[k] = 0;
					}
					else
					{
						for (k = j + 2; k <= j + uthreshold; k++)
						{
							if (p[k] == 0) break;
						}
						if (p[k] == 0)
						{
							for (int h = j + 1; h < k; h++)
								p[h] = 0;
						}
					}
				}
				//列消除
				if (p[j] == 0 && p[j + width] == 255)
				{
					if (i + vthreshold >= height)
					{
						for (k = j + width; k < j + (height - i)*width; k += width)
							p[k] = 0;
					}
					else
					{
						for (k = j + 2 * width; k <= j + vthreshold * width; k += width)
						{
							if (p[k] == 0) break;
						}
						if (p[k] == 0)
						{
							for (int h = j + width; h < k; h += width)
								p[h] = 0;
						}
					}
				}
			}
		}
	}
	imshow("delete_jut", dst);
}

//图片边缘光滑处理
//size表示取均值的窗口大小，threshold表示对均值图像进行二值化的阈值
void imageblur(Mat& src, Mat& dst, Size size, int threshold)
{
	int height = src.rows;
	int width = src.cols;
	blur(src, dst, size);
	for (int i = 0; i < height; i++)
	{
		uchar* p = dst.ptr<uchar>(i);
		for (int j = 0; j < width; j++)
		{
			if (p[j] < threshold)
				p[j] = 0;
			else p[j] = 255;
		}
	}
	imshow("Blur", dst);
}

//int main()
//{
//	cout << "hello world" << endl;
//	Mat grayImage = imread("dst2.png");
//	//Mat grayImage;
//	//resize(image, grayImage,Size(1280,720));
//	Mat dst;
//	Mat alpha = createAlpha(grayImage);
//	//GaussianBlur(alpha, alpha, Size(11, 11), 10, 10);
//	//delete_jut(alpha, alpha, 2, 2, 1);
//	//imageblur(alpha, alpha, Size(3, 3), 200);
//	addAlpha(grayImage, dst, alpha);
//	alpha = complete(dst, dst);
//	imshow("alpha", alpha);
//	imshow("dst", dst);
//	waitKey(0);
//	imwrite("dst.png", dst);
//	imwrite("alpha.png", alpha);
//
//	//Mat img = imread("70.png");
//	//imshow("原始图", img);
//	//Mat DstPic, edge, grayImage;
//
//	////创建与src同类型和同大小的矩阵
//	//DstPic.create(img.size(), img.type());
//
//	////将原始图转化为灰度图
//	//cvtColor(img, grayImage, COLOR_BGR2GRAY);
//
//	////先使用3*3内核来降噪
//	//blur(grayImage, edge, Size(3, 3));
//
//	////运行canny算子
//	//Canny(edge, edge, 3, 9, 3);
//	//GaussianBlur(edge, edge, Size(3, 3), 5, 5);
//	//imshow("边缘提取效果", edge);
//
//	//vector<vector<Point>> g_vContours;
//	//vector<Vec4i> g_vHierarchy;
//
//	//RNG G_RNG(1234);
//
//	//findContours(edge, g_vContours, g_vHierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
//	//Mat Drawing = Mat::zeros(edge.size(), CV_8UC3);
//
//	//for (int i = 0; i < g_vContours.size(); i++)
//	//{
//	//	Scalar color = Scalar(G_RNG.uniform(0, 255), G_RNG.uniform(0, 255), G_RNG.uniform(0, 255));
//	//	drawContours(Drawing, g_vContours, i, color, 2, 8, g_vHierarchy, 0, Point());
//	//}
//	//GaussianBlur(Drawing, Drawing, Size(3, 3), 5, 5);
//	//imshow("效果图", Drawing);
//
//	//waitKey(0);
//
//	//const int MAX_CLUSTERS = 5;
//	//Vec3b colorTab[] =
//	//{
//	//	Vec3b(0, 0, 255),
//	//	Vec3b(0, 255, 0),
//	//	Vec3b(255, 100, 100),
//	//	Vec3b(255, 0, 255),
//	//	Vec3b(0, 255, 255)
//	//};
//	//Mat data, labels;
//	//Mat pic = imread("70.png");
//	//for (int i = 0; i < pic.rows; i++)
//	//	for (int j = 0; j < pic.cols; j++)
//	//	{
//	//		Vec3b point = pic.at<Vec3b>(i, j);
//	//		Mat tmp = (Mat_<float>(1, 3) << point[0], point[1], point[2]);
//	//		data.push_back(tmp);
//	//	}
//
//	////根据浏览图片，确定k=3
//	//kmeans(data, 10, labels, TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 10, 1.0),
//	//	3, KMEANS_RANDOM_CENTERS);
//
//	//int n = 0;
//	////显示聚类结果，不同的类别用不同的颜色显示
//	//for (int i = 0; i < pic.rows; i++)
//	//	for (int j = 0; j < pic.cols; j++)
//	//	{
//	//		int clusterIdx = labels.at<int>(n);
//	//		pic.at<Vec3b>(i, j) = colorTab[clusterIdx];
//	//		n++;
//	//	}
//	//Mat alpha = createAlpha(pic);
//	//Mat dst;
//	//Mat src = imread("70.png");
//	//addAlpha(src, dst, alpha);
//	//imshow("aaa", alpha);
//	//imshow("pic", dst);
//	//waitKey(0);
//	//imwrite("alpha.png", alpha);
//	//imwrite("dst.png", dst);
//
//	return 0;
//}
