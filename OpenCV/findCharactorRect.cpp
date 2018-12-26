#include "stdafx.h"
//#include <opencv2/opencv.hpp>
//#include <bgsegm.hpp>
//#include <string>
//#include <opencv2/tracking.hpp>
//
//using namespace std;
//using namespace cv;
//
//void track();
//void dilate(int index, int length, std::vector< std::vector< cv::Point> > contours, Rect &charactorRect, int i, Mat mask);
//Mat imageFusion(Mat &frame);
//
//int main(){  
//    Mat src,mask,thresholdImage, output, frame;  
//	//frame = imread("E:\\screenshots\\tempNULL\\0.jpg", 1);  
//	//Ptr<BackgroundSubtractorMOG2> bgSubtractor =createBackgroundSubtractorMOG2(20, 16, true);
//	/*Ptr<BackgroundSubtractorMOG2> bgSubtractor =createBackgroundSubtractorMOG2();
//	bgSubtractor->setVarThreshold(20);*/
//	Ptr<bgsegm::BackgroundSubtractorMOG> bgSubtractor = bgsegm::createBackgroundSubtractorMOG(20, 5, 0.5);
//	Ptr<Tracker> tracker = TrackerMIL::create();
//    //BackgroundSubtractorMOG bgSubtractor(20,10,0.5,false);  
//	Rect2d box;
//	int length = 0;
//	int maxGap = 10;
//	for(int i = 34; i <= 69; i++)
//	{
//		cout << i << endl;
//		char temp[8];
//		sprintf_s(temp, "%d.jpg", i);
//		string str = temp;
//		src = imread("E:\\screenshots\\temp\\" + str, 1);  
//		Mat tempImg(src, Rect(400, 150, 200, 200));
//		double maxValue;
//		minMaxLoc(tempImg, NULL, &maxValue);
//		if(maxValue < 10){
//			cout << i << " ºÚÉ«±³¾°" << endl;
//			continue;
//		}
//		src = Mat(src, Rect(0, 206, 795, 313));
//		//src = Mat(src, Rect(0, 84, 795, 432));
//		//cvtColor(src, src, COLOR_BGR2HSV);
//		frame = imageFusion(src);
//		bgSubtractor->apply(frame,mask,0.5);
//		std::vector< std::vector< cv::Point> > contours;  
//		vector<Vec4i> hierarchy;
//		Mat tempMask;
//		mask.copyTo(tempMask);
//		findContours(tempMask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//		int len = contours.size();
//		for(vector<vector< cv::Point>>::iterator it = contours.begin(); it != contours.end();){
//			Rect rect = boundingRect(*it);
//			if(rect.width < 10 || rect.height < 10){
//				contours.erase(it);
//			}else{
//				it++;
//			}
//		}
//		//cout << len << " " << abs(length - len) << endl;
//		//if(abs(length - len) > 1000){
//		//	bgSubtractor->clear();
//		//	bgSubtractor->apply(frame,mask,0.5);
//		//	i--;
//		//	//length = contours.size();
//		//	length = 0;
//		//	continue;
//		//}
//		length = contours.size();
//		Rect charactorRect(0, 0, 0, 0);
//		int index = 0;
//		for(int j = 0; j < length; j++){
//			//cout << hierarchy[i][0] << " " << hierarchy[i][1] << " " << hierarchy[i][2] << " " << hierarchy[i][3] << endl;
//			//cout << hierarchy[i] << endl;
//			Rect rect = boundingRect(contours[j]);
//			if(rect.height > charactorRect.height){
//				//if(j > 0){
//				//	Rect backRect = boundingRect(contours[j - 1]);
//				//	Rect previousRect = boundingRect(contours[j + 1]);
//				//	/*if(abs(rect.y - previousRect.y) >= 50)
//				//		continue;*/
//				//	 if((rect.x > (backRect.x + backRect.width)) && abs(rect.x - (backRect.x + backRect.width)) >= maxGap)
//				//		continue;
//				//	else if((rect.x > (previousRect.x + previousRect.width)) && abs(rect.x - (previousRect.x + previousRect.width)) >= maxGap)
//				//		continue;
//				//	else if((rect.x + rect.width) < backRect.x && abs((rect.x + rect.width) - backRect.x) >= maxGap)
//				//		continue;
//				//	else if((rect.x + rect.width) < previousRect.x && abs((rect.x + rect.width) - previousRect.x) >= maxGap)
//				//		continue;
//				//}
//				charactorRect = rect;
//				index = j;
//			}
//			//cout << rect.x << " " << rect.y << endl;
//			//rectangle(mask, rect, Scalar::all(255));
//		}
//		dilate(index, length, contours, charactorRect, i, mask);
//		if(charactorRect.height < 100){
//			continue;
//		}
//		//cout << charactorRect.height << endl;
//
//		//cout << charactorRect.width << " " << charactorRect.height << endl;
//		//box = Rect2d(charactorRect);
//		//rectangle(mask, box, Scalar::all(255));
//		//if(i == 5){
//		//	Rect charactorRect;
//		//	int minX = 800, minY = 600, maxX = 0, maxY = 0;
//		//	for(int j = 0; j < contours.size(); j++){
//		//		Rect rect = boundingRect(contours[j]);
//		//		if(rect.x < 500){
//		//			if(rect.x < minX)
//		//				minX = rect.x;
//		//			if(rect.y < minY)
//		//				minY = rect.y;
//		//			if((rect.x + rect.width) > maxX)
//		//				maxX = rect.x + rect.width;
//		//			if((rect.y + rect.height) > maxY)
//		//				maxY = rect.y + rect.height;
//		//		}
//		//		//rectangle(mask, rect, Scalar::all(255));
//		//		//cout << rect.x << " " << rect.y << endl;
//		//	}
//		//	box = Rect2d(minX, minY, maxX - minX, maxY - minY);
//		//	cout << box.x << " " << box.y << " " << box.width << " " << box.height << endl;
//		//	//rectangle(mask, Point(minX, minY), Point(maxX, maxY), Scalar(255));
//		//	rectangle(mask, box, Scalar::all(255));
//		//	tracker->init(mask, box);
//		//}
//		//if(i > 5){
//		//	tracker->update(mask, box);
//		//	rectangle(mask, box, Scalar(255, 0, 0), 2, 1);
//		//}
//		cout << "--------------------" << endl;
//		rectangle(mask, charactorRect, Scalar::all(255));
//		//drawContours(mask, contours, -1, Scalar(255));
//        imshow("mask",mask);  
//        waitKey(0); 
//	}
//	//track();
//    return 0;  
//} 
//
//void dilate(int index, int length, std::vector< std::vector< cv::Point> > contours, Rect &charactorRect, int i, Mat mask){
//	int threshold = 10;
//	int maxWidth = -1, maxHeight = -1;
//	int minX = -1, minY = -1;
//	bool xIsLock = false, yIsLock = false;
//	for(int j = index - 1, k = index + 1; (j >= 0 || k < length) ;j--, k++){
//			if(j >= 0){
//				Rect rectLow = boundingRect(contours[j]);
//				if(!(rectLow.x >= charactorRect.x && rectLow.y >= charactorRect.y && (rectLow.x + rectLow.width) <= (charactorRect.x + charactorRect.width)
//					&& (rectLow.y + rectLow.height) <= (charactorRect.y + charactorRect.height))){
//						if((rectLow.y + rectLow.height) <= (charactorRect.y + charactorRect.height)){
//							if(rectLow.x < charactorRect.x){
//								int xLow = rectLow.x + rectLow.width, xCharactor = charactorRect.x + charactorRect.width;
//								if((xLow - charactorRect.x) < 0 && abs(xLow - charactorRect.x) <= threshold){
//									int width = charactorRect.width + abs(rectLow.x - charactorRect.x);
//									charactorRect = Rect((rectLow.x < charactorRect.x ? rectLow.x : charactorRect.x), charactorRect.y, width, charactorRect.height);
//								}else if((xLow - charactorRect.x) >= 0){
//									charactorRect = Rect((rectLow.x < charactorRect.x ? rectLow.x : charactorRect.x), 
//										charactorRect.y, abs(rectLow.x - (xLow > xCharactor ? xLow : xCharactor)), charactorRect.height);
//								}
//							}else if(rectLow.x <= (charactorRect.x + charactorRect.width)){
//								charactorRect = Rect(charactorRect.x, charactorRect.y, abs(charactorRect.x - (rectLow.x + rectLow.width)), charactorRect.height);
//							}else if(abs(rectLow.x - (charactorRect.x + charactorRect.width)) <= threshold){
//								charactorRect = Rect(charactorRect.x, charactorRect.y, abs(charactorRect.x - (rectLow.x + rectLow.width)), charactorRect.height);
//							}
//						}else if(rectLow.y <= (charactorRect.y + charactorRect.height)){
//							if(rectLow.x < charactorRect.x){
//								int xLow = rectLow.x + rectLow.width, xCharactor = charactorRect.x + charactorRect.width;
//								if((xLow - charactorRect.x) < 0 && abs(xLow - charactorRect.x) <= threshold){
//									int width = charactorRect.width + abs(rectLow.x - charactorRect.x);
//									charactorRect = Rect(rectLow.x, charactorRect.y, width, abs(charactorRect.y - (rectLow.y + rectLow.height)));
//								}else if((xLow - charactorRect.x) >= 0){
//									charactorRect = Rect(rectLow.x, charactorRect.y, abs(rectLow.x - (xLow > xCharactor ? xLow : xCharactor)), 
//										abs(charactorRect.y - (rectLow.y + rectLow.height)));
//								}
//							}else if(rectLow.x <= (charactorRect.x + charactorRect.width)){
//								if((rectLow.x + rectLow.width) < (charactorRect.x + charactorRect.width))
//									charactorRect = Rect(charactorRect.x, charactorRect.y, charactorRect.width, 
//									abs(charactorRect.y - (rectLow.y + rectLow.height)));
//								else
//									charactorRect = Rect(charactorRect.x, charactorRect.y, abs(charactorRect.x - (rectLow.x + rectLow.width)), 
//										abs(charactorRect.y - (rectLow.y + rectLow.height)));
//							}else if(abs(rectLow.x - (charactorRect.x + charactorRect.width)) <= threshold){
//								charactorRect = Rect(charactorRect.x, charactorRect.y, abs(charactorRect.x - (rectLow.x + rectLow.width)), 
//									abs(charactorRect.y - (rectLow.y + rectLow.height)));
//							}
//						}else{
//							int rectRight_x = rectLow.x + rectLow.width, rectBelow_y = rectLow.y + rectLow.height;
//							int charactorRight_x = charactorRect.x + charactorRect.width, charactorBelow_y = charactorRect.y + charactorRect.height;
//							if(rectLow.x < charactorRect.x){
//								if((charactorRect.x - rectRight_x) <= threshold && abs(rectLow.y - charactorBelow_y) <= threshold){
//									charactorRect = Rect(rectLow.x, charactorRect.y, 
//										abs(rectLow.x - (rectRight_x > charactorRight_x ? rectRight_x : charactorRight_x)), 
//										charactorRect.height + abs(charactorBelow_y - rectBelow_y));
//								}
//							}else if(rectLow.x - (charactorRect.x + charactorRect.width) <= threshold){
//								if(abs(rectLow.y - charactorBelow_y) <= threshold){
//									charactorRect = Rect(charactorRect.x, charactorRect.y, 
//										rectRight_x > charactorRight_x ? abs(charactorRect.x - rectRight_x) : charactorRect.width, 
//										charactorRect.height + abs(charactorBelow_y - rectBelow_y));
//								}
//							}
//						}
//				}
//			}
//			/*if(i == 31){
//				cout << charactorRect.width << " " << charactorRect.height << endl;
//				rectangle(mask, charactorRect, Scalar::all(255));
//				imshow("mask",mask);  
//				waitKey(0); 
//			}*/
//			if(charactorRect.width >= 100){
//				if(!xIsLock){
//					
//					maxWidth = charactorRect.width;
//					minX = charactorRect.x;
//					xIsLock = true;
//				}
//			}
//			if(charactorRect.height >= 100){
//				if(!yIsLock){
//					maxHeight = charactorRect.height;
//					minY = charactorRect.y;
//					yIsLock = true;
//				}
//			}
//			if(k < length){
//				Rect rectHigh = boundingRect(contours[k]);
//				if(!(rectHigh.x >= charactorRect.x && rectHigh.y >= charactorRect.y && (rectHigh.x + rectHigh.width) <= (charactorRect.x + charactorRect.width)
//					&& (rectHigh.y + rectHigh.height) <= (charactorRect.y + charactorRect.height))){
//					int rectRight_x = rectHigh.x + rectHigh.width, rectBelow_y = rectHigh.y + rectHigh.height;
//					int charactorRight_x = charactorRect.x + charactorRect.width, charactorBelow_y = charactorRect.y + charactorRect.height;
//					if(rectHigh.y == charactorRect.y){
//						/*cout << rectHigh.x << " " << rectHigh.y << " " << rectHigh.width << " " << rectHigh.height <<endl;
//						cout << charactorRect.x << " " << charactorRect.y << " " << charactorRect.width << " " << charactorRect.height <<endl;
//						system("pause");*/
//						if((charactorRect.x - rectRight_x) <= threshold){
//							charactorRect = Rect(rectHigh.x < charactorRect.x ? rectHigh.x : charactorRect.x, rectHigh.y, 
//								abs((rectHigh.x < charactorRect.x ? rectHigh.x : charactorRect.x) - (rectRight_x > charactorRight_x ? rectRight_x : charactorRight_x)),
//								abs(rectHigh.y - (rectBelow_y > charactorBelow_y ? rectBelow_y : charactorBelow_y)));
//						}
//					}else if(rectBelow_y > charactorRect.y){
//						if(rectHigh.x < charactorRect.x){
//							if((charactorRect.x - rectRight_x) <= threshold){
//								charactorRect = Rect(rectHigh.x, rectHigh.y, abs(rectHigh.x - (rectRight_x > charactorRight_x ? rectRight_x : charactorRight_x)), 
//									abs(rectHigh.y - (charactorBelow_y > rectBelow_y ? charactorBelow_y : rectBelow_y)));
//							}
//						}else if((rectHigh.x - charactorRight_x) <= threshold){
//							charactorRect = Rect(charactorRect.x, rectHigh.y, abs(charactorRect.x - (rectRight_x > charactorRight_x ? rectRight_x : charactorRight_x)),
//								abs(rectHigh.y - (charactorBelow_y > rectBelow_y ? charactorBelow_y : rectBelow_y)));
//						}
//					}else{
//						if(abs(rectBelow_y - charactorRect.y) <= threshold){
//							if(rectHigh.x < charactorRect.x){
//								if((charactorRect.x - rectRight_x) <= threshold){
//									charactorRect = Rect(rectHigh.x, rectHigh.y, abs(rectHigh.x - (rectRight_x > charactorRight_x ? rectRight_x : charactorRight_x)), 
//										abs(rectHigh.y - (charactorBelow_y > rectBelow_y ? charactorBelow_y : rectBelow_y)));
//								}
//							}else if((rectHigh.x - charactorRight_x) <= threshold){
//								charactorRect = Rect(charactorRect.x, rectHigh.y, 
//									abs(charactorRect.x - (rectRight_x > charactorRight_x ? rectRight_x : charactorRight_x)),
//									abs(rectHigh.y - (charactorBelow_y > rectBelow_y ? charactorBelow_y : rectBelow_y)));
//							}
//						}
//					}
//				}
//			}
//			if(charactorRect.width >= 100){
//				if(!xIsLock){
//					maxWidth = charactorRect.width;
//					minX = charactorRect.x;
//					xIsLock = true;
//				}
//			}
//			if(charactorRect.height >= 100){
//				if(!yIsLock){
//					maxHeight = charactorRect.height;
//					minY = charactorRect.y;
//					yIsLock = true;
//				}
//			}
//			/*if(i == 31){
//				cout << charactorRect.width << " " << charactorRect.height << endl;
//				rectangle(mask, charactorRect, Scalar::all(255));
//				imshow("mask",mask);  
//				waitKey(0); 
//			}*/
//			if(charactorRect.width >= 100 && charactorRect.height >= 100)
//				break;
//		}
//		if(maxWidth != -1 && minX != -1){
//			charactorRect.width = maxWidth;
//			charactorRect.x = minX;
//		}
//		if(maxHeight != -1 && minY != -1){
//			charactorRect.height = maxHeight;
//			charactorRect.y = minY;
//		}
//}
//
//void track(){
//  //Ptr<Tracker> tracker = TrackerBoosting::create();
//  Ptr<Tracker> tracker = TrackerMIL::create();
//  //Ptr<Tracker> tracker = TrackerMedianFlow::create();
//  //Ptr<Tracker> tracker = TrackerKCF::create();
//
//  Mat frame;
//  frame = imread("E:\\screenshots\\source\\0.jpg", 1);  
//  //frame = imread("pic\\temp\\0.jpg", 1);  
//
//  Rect2d box = selectROI("Tracking", frame);
//  //cout << box.x << " " << box.y << " " << box.width << " " << box.height << endl;
//  //Rect2d box(270, 120, 180, 260);
//  tracker->init(frame, box);
//  for(int i = 0; i <= 67; i++)
//	{
//		char temp[8];
//		sprintf_s(temp, "%d.jpg", i);
//		string str = temp;
//		frame = imread("E:\\screenshots\\source\\" + str, 1);  
//		//frame = imread("pic\\temp\\" + str, 1);  
//		tracker->update(frame, box);
//		rectangle(frame, box, Scalar(255, 0, 0), 2, 1);
//		imshow("img",frame);  
//		cv::waitKey(1);  
//	}
//}