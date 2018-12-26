#include "stdafx.h"
#include <iostream>
#include <opencv2\opencv.hpp>
#include "VirtualKeyBoard.h"
#include "Keys.h"
#include <Windows.h>
#include <fstream>

using namespace std;
using namespace cv;

Point2f judgeButton(string srcNum);
Point2f judgeButton(string srcNum, string doorNum);
//void snapShot(char filename[]);
void snapShot(cv::Mat& image);
double light(IplImage * image);
Point2f tracking(Mat &frame);

class KeyTime{
public: 
	string key;
	long pressTime;
	long startTime;
	KeyTime(string key, long pressTime, long startTime){
		this->key = key;
		this->pressTime = pressTime;
		this->startTime = startTime;
	}
};

extern int matchesThreshold;
extern int featureNum;
int roomNum = 1; // 范围1-7;
int runNum = 1;
map<string, int> keys;
long originTime;
bool isRunning = true;
bool isPressKey = true;
int sign = 1;
int key = 2;
int startTime = 3;
int endTime = 4;
map<int, Point2f> keyMemory_adjustPosition;

string pictureName;

void init(){
	keys.insert(pair<string, int>("Q", Keys::Q));
	keys.insert(pair<string, int>("W", Keys::W));
	keys.insert(pair<string, int>("E", Keys::E));
	keys.insert(pair<string, int>("R", Keys::R));
	keys.insert(pair<string, int>("T", Keys::T));
	keys.insert(pair<string, int>("A", Keys::A));
	keys.insert(pair<string, int>("S", Keys::S));
	keys.insert(pair<string, int>("D", Keys::D));
	keys.insert(pair<string, int>("F", Keys::F));
	keys.insert(pair<string, int>("G", Keys::G));
	keys.insert(pair<string, int>("H", Keys::H));
	keys.insert(pair<string, int>("Z", Keys::Z));
	keys.insert(pair<string, int>("X", Keys::X));
	keys.insert(pair<string, int>("C", Keys::C));
	keys.insert(pair<string, int>("V", Keys::V));
	keys.insert(pair<string, int>("SPACE", Keys::SPACE));
	keys.insert(pair<string, int>("ESC", Keys::ESC));
	keys.insert(pair<string, int>("F10", Keys::F10));
	keys.insert(pair<string, int>("F12", Keys::F12));
	keys.insert(pair<string, int>("UP", Keys::UP));
	keys.insert(pair<string, int>("LEFT", Keys::LEFT));
	keys.insert(pair<string, int>("DOWN", Keys::DOWN));
	keys.insert(pair<string, int>("RIGHT", Keys::RIGHT));
	keys.insert(pair<string, int>("RIGHT_CTRL", Keys::RIGHT_CTRL));
	keys.insert(pair<string, int>("2", Keys::NUM_2));
	keys.insert(pair<string, int>("6", Keys::NUM_6));
}

bool isPress(int key){
	int val = getAsyncKeyState(key);
	if((val & 0x8000) == 32768){
		return true;
	}
	return false;
}

bool isRestart(){
	if(isPress('0')){
		return true;
	}
	return false;
}

bool isPause(){
	if(isPress(VK_HOME)){
		return true;
	}
	return false;
}

bool isQuit(){
	if(isPress(VK_END)){
		isRunning = false;
		return true;
	}
	return false;
}

bool isExist(Point2f point){
	if(point.x + 1.0 <= 1e-5 && point.y + 1.0 <= 1e-5){
		return false;
	}
	return true;
}

Point2f getPoint(string imgName){
	pictureName = imgName;
	Point2f point = judgeButton(imgName);
	if(point.x == -1 && point.y == -1){
		return point;
	}
	SYSTEMTIME st;
	GetLocalTime(&st);
	originTime = st.wHour*60*60*1000 + st.wMinute*60*1000 + st.wSecond*1000 + st.wMilliseconds;
	return point;
}
	
Point2f getPoint(string imgName, string doorNum){
	Point2f point = judgeButton(imgName, doorNum);
	if(point.x == -1 && point.y == -1){
		return point;
	}
	SYSTEMTIME st;
	GetLocalTime(&st);
	originTime = st.wHour*60*60*1000 + st.wMinute*60*1000 + st.wSecond*1000 + st.wMilliseconds;
	return point;
}

void runAs(Point2f point, float k1 = 7.5f, float k2 = 5.0f, bool isRun = false){
	vector<KeyTime> time;
	string key_x, key_y;
	long time_x = 0, time_y = 0;
	if(point.x < -2.0){
		key_x = "LEFT";
		time_x = (long)k1 * abs(point.x);
	}else if(point.x > 2.0){
		key_x = "RIGHT";
		time_x = (long)k1 * abs(point.x);
	}
	if(point.y < -2.0){
		key_y = "UP";
		time_y = (long)k2 * abs(point.y);
	}else if(point.y > 2.0){
		key_y = "DOWN";
		time_y = (long)k2 * abs(point.y);
	}
	if(time_y != 0){
		SYSTEMTIME st;
		GetLocalTime(&st);
		long currentTime = st.wHour*60*60*1000 + st.wMinute*60*1000 + st.wSecond*1000 + st.wMilliseconds;
		KeyDown(keys[key_y]);
		time.push_back(KeyTime(key_y, time_y, currentTime));
	}
	if(time_x != 0){
		SYSTEMTIME st;
		GetLocalTime(&st);
		long currentTime = st.wHour*60*60*1000 + st.wMinute*60*1000 + st.wSecond*1000 + st.wMilliseconds;
		if(isRun){
			KeyPress(keys[key_x]);
		}
		KeyDown(keys[key_x]);
		time.push_back(KeyTime(key_x, time_x, currentTime));
	}
	while(!time.empty()){
		for(vector<KeyTime>::iterator it = time.begin(); it != time.end(); ){
			KeyTime keyTime = *it;
			SYSTEMTIME st;
			GetLocalTime(&st);
			long currentTime = st.wHour*60*60*1000 + st.wMinute*60*1000 + st.wSecond*1000 + st.wMilliseconds;
			if(currentTime - keyTime.startTime >= keyTime.pressTime){
				KeyUp(keys[keyTime.key]);
				it = time.erase(it);
				interval();
			}else{
				++it;
			}
		}
	}
	cout << key_x << " " << time_x << endl;
	cout << key_y << " " << time_y << endl;
}

void keyPress(){
	cout << "执行到keyPress里面" << endl;
	ostringstream ss;
	ss << "E:\\screenshots\\key\\" << roomNum << ".txt";
	fstream file(ss.str());
	bool isLoop = true;
	vector<string> temp;
	vector<KeyTime> time;
	char buffer[100];
	while(true){
		if(isQuit()) return;
		if(isPause()) system("pause");
		if(time.empty() && !isLoop){
			file.close();
			return;
		}
		for(vector<KeyTime>::iterator it = time.begin(); it != time.end(); ){
			KeyTime keyTime = *it;
			SYSTEMTIME st;
			GetLocalTime(&st);
			long currentTime = st.wHour*60*60*1000 + st.wMinute*60*1000 + st.wSecond*1000 + st.wMilliseconds;
			if(currentTime - keyTime.startTime >= keyTime.pressTime){
				KeyUp(keys[keyTime.key]);
				it = time.erase(it);
				interval();
			}else{
				++it;
			}
		}
		if(!temp.empty()){
			SYSTEMTIME st;
			GetLocalTime(&st);
			long currentTime = st.wHour*60*60*1000 + st.wMinute*60*1000 + st.wSecond*1000 + st.wMilliseconds;
			long tempStartTime;
			istringstream is;
			is.str(temp[startTime]);
			is >> tempStartTime;
			if(currentTime - originTime < tempStartTime){
				continue;
			}
			long tempEndTime;
			is.clear();
			is.str(temp[endTime]);
			is >> tempEndTime;
			if(temp[sign].compare("0") == 0){
				KeyPress(keys[temp[key]], tempEndTime - tempStartTime);
				temp.clear();
			}else{
				KeyDown(keys[temp[key]]);
				time.push_back(KeyTime(temp[key], tempEndTime - tempStartTime, currentTime));
				interval();
				temp.clear();
			}
		}
		if(!file.eof()){
			while(true){
				file.getline(buffer, 100);
				if(!(buffer[0] == '/' && buffer[1] == '/')){
					break;
				}
			}
			char *p;
			p = strtok(buffer, " ");
			while( p != NULL )
			{
				string str = p;
				temp.push_back(str);
				p = strtok( NULL, " " );
			}
		}
		if(file.eof()){
			isLoop = false;
		}
		if(!file.eof()){
			SYSTEMTIME st;
			GetLocalTime(&st);
			long currentTime = st.wHour*60*60*1000 + st.wMinute*60*1000 + st.wSecond*1000 + st.wMilliseconds;
			long tempStartTime;
			istringstream is;
			is.str(temp[startTime]);
			is >> tempStartTime;
			if(currentTime - originTime < tempStartTime){
				continue;
			}
			long tempEndTime;
			is.clear();
			is.str(temp[endTime]);
			is >> tempEndTime;
			if(temp[sign].compare("0") == 0){
				KeyPress(keys[temp[key]], tempEndTime - tempStartTime);
				temp.clear();
			}else{
				KeyDown(keys[temp[key]]);
				time.push_back(KeyTime(temp[key], tempEndTime - tempStartTime, currentTime));
				interval();
				temp.clear();
			}
		}
	}
}

void attackMonster(){
	if(roomNum == 7){
		KeyPress(keys["V"]);
		KeyPress(keys["V"]);
		KeyPress(keys["V"]);
	}else if(roomNum == 6){
		KeyPress(keys["D"]);
	}else if(roomNum == 5){
		KeyPress(keys["D"]);
	}else if(roomNum == 4){
		KeyPress(keys["D"]);
	}else if(roomNum == 3){
		KeyPress(keys["Q"]);
	}else if(roomNum == 2){
		KeyPress(keys["D"]);
	}else{
		KeyPress(keys["D"]);
		KeyPress(keys["X"], 1000);
	}
}

bool findMonster(){
	cout << "执行到findMonster" << endl;
	interval(500);
	Mat imgScene;
	snapShot(imgScene);
	IplImage temp(imgScene);
	double a = light(&temp);
	if(a < -118.0){
		return false;
	}
	tracking(imgScene);
	//KeyPress(keys["RIGHT"]);
	for(int i = 0; i < 3; i++){
		OUTLOOP:
		snapShot(imgScene);
		IplImage temp(imgScene);
		double a = light(&temp);
		if(a < -118.0){
			continue;
		}
		Point2f monsterPoint = tracking(imgScene);
		if(monsterPoint.x == -1 && monsterPoint.y == -1){
			continue;
		}
		KeyPress(keys["RIGHT"]);
		cout << "monster坐标：" << monsterPoint << endl;
		Point2f characterPoint = getPoint("character\\left.jpg");
		if(roomNum == 1 && !isExist(characterPoint)){
			continue;
		}
		int k = 0;
		while(!isExist(characterPoint)){
			if(k > 5){
				if(i >= 3) return false;
				i++;
				goto OUTLOOP;
			}
			k++;
			cout << "角色没找到" << endl;
			characterPoint = getPoint("character\\left.jpg");
		}
		cout << "character坐标：" << characterPoint << endl;
		if(abs(monsterPoint.x - characterPoint.x) < 35/* || abs(monsterPoint.y - characterPoint.y) < 25*/){
			cout << "距离太近，没有怪" << endl;
			continue;
		}
		cout << "角色和目标位置差：" << monsterPoint - characterPoint << endl;
		runAs(monsterPoint - characterPoint, 5.5f);
		//KeyPress(keys["D"]);
		attackMonster();
		//KeyPress(keys["X"], 2000);
		runAs(characterPoint - monsterPoint, 5.5f);
		return true;
	}
	if(roomNum == 6){
		Point2f monsterPoint = getPoint("monster\\monster2.jpg");
		if(!isExist(monsterPoint)){
			return false;
		}
		Point2f characterPoint;
		for(int i = 0; i < 3; i++){
			KeyPress(keys["RIGHT"]);
			characterPoint = getPoint("character\\left.jpg");
			if(isExist(characterPoint)){
				runAs(monsterPoint - characterPoint, 5.5f);
				attackMonster();
				runAs(characterPoint - monsterPoint, 5.5f);
				return true;
			}
		}
	}
	return false;
}

void walkToDoor(){
	if(roomNum == 2 || roomNum == 5){
		if(roomNum == 2){
			KeyPress(keys["UP"], 300);
		}
		bool isFindDoor = true;
		Point2f pointDoor;
		int findDoorNum = 0;
		SYSTEMTIME st;
		GetLocalTime(&st);
		long origin = st.wHour*60*60*1000 + st.wMinute*60*1000 + st.wSecond*1000 + st.wMilliseconds;
		long current = origin;
		KeyDown(keys["RIGHT"]);
		while(true){
			if(findDoorNum > 5 && current - origin > 2500){
				if(isExist(getPoint("rightCloseDoor.jpg")) || current - origin > 3500){
					KeyUp(keys["RIGHT"]);
					findDoorNum = 0;
					if(findMonster()){
						KeyPress(keys["LEFT"], 1000);
						continue;
					}
					KeyPress(keys["LEFT"], 2500);
					if(roomNum == 2){
						KeyPress(keys["DOWN"], 300);
					}
					findMonster();
					SYSTEMTIME st1;
					GetLocalTime(&st1);
					origin = st1.wHour*60*60*1000 + st1.wMinute*60*1000 + st1.wSecond*1000 + st1.wMilliseconds;
					if(roomNum == 2){
						KeyPress(keys["UP"], 300);
					}
					KeyDown(keys["RIGHT"]);
				}
			}
			if(isFindDoor){
				pointDoor = getPoint("rightDoor.jpg");
				findDoorNum++;
				SYSTEMTIME st1;
				GetLocalTime(&st1);
				current = st1.wHour*60*60*1000 + st1.wMinute*60*1000 + st1.wSecond*1000 + st1.wMilliseconds;
				if(pointDoor.x + 1.0 <= 1e-5 && pointDoor.y + 1.0 <= 1e-5){
					continue;
				}
				/*int tempFeatureNum = featureNum;
				getPoint("belowCloseDoor.jpg");
				if(featureNum > tempFeatureNum){
					continue;
				}*/
				findDoorNum = 0;
				isFindDoor = false;
			}
			KeyUp(keys["RIGHT"]);
			KeyPress(keys["RIGHT"]);
			Point2f pointCharactor = getPoint("character\\left1.jpg");
			if(pointCharactor.x + 1.0 <= 1e-5 && pointCharactor.y + 1.0 <= 1e-5){
				continue;
			}
			//cout << pointDoor << pointCharactor << endl;
			runAs(pointDoor - pointCharactor, 6.0f);
			break;
		}
	}else if(roomNum == 3 || roomNum == 4){
		bool isFindDoor = true;
		Point2f pointDoor;
		int findDoorNum = 0;
		SYSTEMTIME st;
		GetLocalTime(&st);
		long origin = st.wHour*60*60*1000 + st.wMinute*60*1000 + st.wSecond*1000 + st.wMilliseconds;
		long current = origin;
		if(roomNum == 3){
			findMonster();
		}
		int time = 300;
		if(roomNum == 3){
			KeyPress(keys["LEFT"], 700);
		}else{
			time = 700;
			KeyPress(keys["RIGHT"], 500);
			KeyPress(keys["LEFT"]);
		}
		KeyDown(keys["DOWN"]);
		while(true){
			if(findDoorNum > 5 && current - origin > time){
				KeyUp(keys["DOWN"]);
				findDoorNum = 0;
				if(roomNum == 4){
					if(findMonster()){
						continue;
					}
				}
				KeyPress(keys["UP"], time);
				if(roomNum == 3){
					KeyPress(keys["RIGHT"], 500);
				}
				findMonster();
				SYSTEMTIME st1;
				GetLocalTime(&st1);
				origin = st1.wHour*60*60*1000 + st1.wMinute*60*1000 + st1.wSecond*1000 + st1.wMilliseconds;
				if(roomNum == 3){
					KeyPress(keys["LEFT"], 500);
				}else{
					KeyPress(keys["LEFT"]);
				}
				KeyDown(keys["DOWN"]);
			}
			if(isFindDoor){
				pointDoor = getPoint("belowDoor.jpg"); 
				findDoorNum++;
				SYSTEMTIME st1;
				GetLocalTime(&st1);
				current = st1.wHour*60*60*1000 + st1.wMinute*60*1000 + st1.wSecond*1000 + st1.wMilliseconds;
				if(pointDoor.x + 1.0 <= 1e-5 && pointDoor.y + 1.0 <= 1e-5){
					continue;
				}
				int tempFeatureNum = featureNum;
				getPoint("belowCloseDoor.jpg");
				if(featureNum > tempFeatureNum){
					continue;
				}
				findDoorNum = 0;
				isFindDoor = false;
			}
			KeyUp(keys["DOWN"]);
			KeyPress(keys["LEFT"]);
			Point2f pointCharactor = getPoint("character\\right1.jpg");
			if(pointCharactor.x + 1.0 <= 1e-5 && pointCharactor.y + 1.0 <= 1e-5){
				continue;
			}
			runAs(pointDoor - pointCharactor, 6.0f, 6.0f);
			break;
		}
	}else if(roomNum == 6){
		KeyPress(keys["RIGHT"]);
		KeyDown(keys["UP"]);
		bool isFindDoor = true;
		Point2f pointDoor;
		int findDoorNum = 0;
		while(true){
			if(findDoorNum > 5){
				KeyUp(keys["UP"]);
				findDoorNum = 0;
				findMonster();
				KeyDown(keys["UP"]);
			}
			if(isFindDoor){
				pointDoor = getPoint("aboveDoor.jpg");
				findDoorNum++;
				if(pointDoor.x + 1.0 <= 1e-5 && pointDoor.y + 1.0 <= 1e-5){
					continue;
				}
				if(featureNum < 40){
					continue;
				}
				/*int tempFeatureNum = featureNum;
				getPoint("aboveCloseDoor.jpg");
				if(featureNum > tempFeatureNum){
					continue;
				}*/
				findDoorNum = 0;
				isFindDoor = false;
			}
			KeyUp(keys["UP"]);
			KeyPress(keys["RIGHT"]);
			Point2f pointCharactor = getPoint("character\\left1.jpg");
			if(pointCharactor.x + 1.0 <= 1e-5 && pointCharactor.y + 1.0 <= 1e-5){
				continue;
			}
			runAs(pointDoor - pointCharactor, 6.0f);
			//KeyPress(keys["UP"], 300);
			break;
		}
	}else if(roomNum == 1){
		findMonster();
		KeyPress(keys["RIGHT"]);
		KeyPress(keys["RIGHT"], 600);
		if(!isExist(getPoint("leftCloseDoor.jpg"))){
			runAs(getPoint("oneRightDoor.jpg") - getPoint("character\\left1.jpg"), 6.0f);
		}
	}
	else{
		findMonster();
	}
}

void walkToDoor1(){
	if(roomNum == 2 || roomNum == 5){
		if(roomNum == 2){
			KeyPress(keys["UP"], 300);
		}
		//KeyPress(keys["RIGHT"]);
		KeyDown(keys["RIGHT"]);
		bool isFindDoor = true;
		Point2f pointDoor;
		while(true){
			if(isFindDoor){
				pointDoor = getPoint("rightDoor.jpg");
				if(pointDoor.x + 1.0 <= 1e-5 && pointDoor.y + 1.0 <= 1e-5){
					continue;
				}
				isFindDoor = false;
			}
			KeyUp(keys["RIGHT"]);
			Point2f pointCharactor = getPoint("character\\left1.jpg");
			if(pointCharactor.x + 1.0 <= 1e-5 && pointCharactor.y + 1.0 <= 1e-5){
				continue;
			}
			cout << pointDoor << pointCharactor << endl;
			runAs(pointDoor - pointCharactor, 6.0f);
			break;
		}
	}else if(roomNum == 3 || roomNum == 4){
		if(roomNum == 3){
			KeyPress(keys["LEFT"], 500);
		}else{
			KeyPress(keys["LEFT"]);
		}
		KeyDown(keys["DOWN"]);
		bool isFindDoor = true;
		Point2f pointDoor;
		while(true){
			if(isFindDoor){
				pointDoor = getPoint("belowDoor.jpg"); 
				if(pointDoor.x + 1.0 <= 1e-5 && pointDoor.y + 1.0 <= 1e-5){
					continue;
				}
				isFindDoor = false;
			}
			KeyUp(keys["DOWN"]);
			Point2f pointCharactor = getPoint("character\\right1.jpg");
			if(pointCharactor.x + 1.0 <= 1e-5 && pointCharactor.y + 1.0 <= 1e-5){
				continue;
			}
			runAs(pointDoor - pointCharactor, 6.0f, 6.0f);
			break;
		}
	}else if(roomNum == 6){
		KeyPress(keys["RIGHT"]);
		KeyDown(keys["UP"]);
		bool isFindDoor = true;
		Point2f pointDoor;
		while(true){
			if(isFindDoor){
				pointDoor = getPoint("aboveDoor.jpg");
				if(pointDoor.x + 1.0 <= 1e-5 && pointDoor.y + 1.0 <= 1e-5){
					continue;
				}
				isFindDoor = false;
			}
			KeyUp(keys["UP"]);
			Point2f pointCharactor = getPoint("character\\left1.jpg");
			if(pointCharactor.x + 1.0 <= 1e-5 && pointCharactor.y + 1.0 <= 1e-5){
				continue;
			}
			runAs(pointDoor - pointCharactor, 6.0f);
			//KeyPress(keys["UP"], 300);
			break;
		}
	}else if(roomNum == 1){
		KeyPress(keys["RIGHT"]);
		KeyPress(keys["RIGHT"], 600);
	}
	//interval(500);
}

void sell(){
	mouseMoveAndPress(298, 647);
	interval(200);
	mouseMoveAndPress(721, 402);
	mouseMoveAndPress(721, 402);
	mouseMoveAndPress(752, 404);
	mouseMoveAndPress(752, 404);
	mouseMoveAndPress(779, 406);
	mouseMoveAndPress(779, 406);
	mouseMoveAndPress(812, 406);
	mouseMoveAndPress(812, 406);
	mouseMoveAndPress(838, 401);
	mouseMoveAndPress(838, 401);
	mouseMoveAndPress(919, 616);
}

void sleepTime(){
	switch (roomNum)
	{
	case 1:
		interval(4786 - 3707);
		break;
	case 2:
		interval(2027 - 1033);
		break;
	case 3:
		interval(4356 - 1095);
		break;
	case 4:
		interval(1500);
		break;
	case 5:
		interval(5563 - 1235);
		break;
	case 6:
		interval(3996 - 2501);
		break;
	default:
		break;
	}
}

void isExistMonster(){
	Point2f point;
	switch (roomNum)
	{
	case 3:
		point = getPoint("monster\\monster2.jpg");
		if(!isExist(point)){
			break;
		}
		while(true){
			Point2f pointCharactor = getPoint("character\\right.jpg");
			if(!isExist(pointCharactor)){
				pointCharactor = getPoint("character\\left.jpg");
				if(!isExist(pointCharactor)){
					continue;
				}
			}
			Point2f p = point - pointCharactor;
			runAs(p, 5.5f);
			KeyPress(keys["D"]);
			KeyPress(keys["S"]);
			//KeyPress(keys["X"], 2000);
			runAs(-p, 5.5f);
			break;
		}
		break;
	case 6:
		point = getPoint("monster\\monster2.jpg");
		if(!isExist(point)){
			break;
		}
		while(true){
			Point2f pointCharactor = getPoint("character\\left.jpg");
			if(!isExist(pointCharactor)){
				pointCharactor = getPoint("character\\right.jpg");
				if(!isExist(pointCharactor)){
					continue;
				}
			}
			Point2f p = point - pointCharactor;
			runAs(p, 5.5f);
			KeyPress(keys["Q"]);
			//KeyPress(keys["X"], 2000);
			runAs(-p, 5.5f);
			break;
		}
		break;
	default:
		break;
	}
}

void isExistMonster1(){
	Point2f point;
	switch (roomNum)
	{
	case 2:
		point = getPoint("monster\\monster5Left.jpg");
		if(!isExist(point)){
			point = getPoint("monster\\monster5Right.jpg");
			if(!isExist(point)){
				break;
			}
		}
		while(true){
			Point2f pointCharactor = getPoint("character\\left.jpg");
			if(!isExist(pointCharactor)){
				pointCharactor = getPoint("character\\right.jpg");
				if(!isExist(pointCharactor)){
					continue;
				}
			}
			Point2f p = point - pointCharactor;
			runAs(p, 5.5f);
			KeyPress(keys["D"]);
			//KeyPress(keys["X"], 2000);
			runAs(-p, 5.5f);
			break;
		}
		break;
	case 3:
		point = getPoint("monster\\monster2.jpg");
		if(!isExist(point)){
			point = getPoint("monster\\monster6Left.jpg");
			if(!isExist(point)){
				point = getPoint("monster\\monster6Right.jpg");
				if(!isExist(point)){
					break;
				}
			}
		}
		while(true){
			Point2f pointCharactor = getPoint("character\\right.jpg");
			if(!isExist(pointCharactor)){
				pointCharactor = getPoint("character\\left.jpg");
				if(!isExist(pointCharactor)){
					continue;
				}
			}
			Point2f p = point - pointCharactor;
			runAs(p, 5.5f);
			KeyPress(keys["D"]);
			//KeyPress(keys["X"], 2000);
			runAs(-p, 5.5f);
			break;
		}
		break;
	case 6:
		//point = getPoint("monster\\monster2-1.jpg");
		//if(!isExist(point)){
		point = getPoint("monster\\monster2.jpg");
		if(!isExist(point)){
			point = getPoint("monster\\monster1Left.jpg");
			if(!isExist(point)){
				point = getPoint("monster\\monster1Right.jpg");
				if(!isExist(point)){
					break;
				}
			}
		}
		//}
		while(true){
			Point2f pointCharactor = getPoint("character\\left.jpg");
			if(!isExist(pointCharactor)){
				pointCharactor = getPoint("character\\right.jpg");
				if(!isExist(pointCharactor)){
					continue;
				}
			}
			Point2f p = point - pointCharactor;
			runAs(p, 5.5f);
			KeyPress(keys["D"]);
			//KeyPress(keys["X"], 2000);
			runAs(-p, 5.5f);
			break;
		}
		break;
	case 7:
		point = getPoint("monster\\bossRight.jpg");
		if(!isExist(point)){
			point = getPoint("monster\\bossLeft.jpg");
			if(!isExist(point)){
				break;
			}
		}
		for(int i = 0; i < 10; i++){
			KeyPress(keys["V"]);
		}
		interval(5000);
		break;
	default:
		break;
	}
}

void onceAgain(){
	cout << "onceAgain" << endl;
	interval(2000);
	KeyPress(keys["RIGHT_CTRL"]);
	KeyDown(keys["X"]);
	interval(2500);
	if(runNum <= 10){
		KeyPress(keys["2"]);
		KeyPress(keys["6"]);
	}
	interval(3500);
	KeyUp(keys["X"]);
	interval(3000);
	sell();
	KeyPress(keys["ESC"]);
	roomNum = 1;
	runNum++;
	if(runNum > 27){
		runNum = 1;
		cout << "角色完毕！" << endl;
		system("pause");
		return;
	}
	KeyPress(keys["F10"]);
	interval(8000);
}

void gameManager1(){
	if(roomNum == 2 || roomNum == 3 || roomNum == 6){
		Point2f pointCharactor1 = getPoint("leftCloseDoor.jpg");
		if(pointCharactor1.x + 1.0 <= 1e-5 && pointCharactor1.y + 1.0 <= 1e-5){
			return;
		}
	}else if(roomNum == 4){
		Point2f pointCharactor1 = getPoint("character\\left.jpg");
		if(pointCharactor1.x + 1.0 <= 1e-5 && pointCharactor1.y + 1.0 <= 1e-5){
			return;
		}
		if(isExist(getPoint("belowDoor.jpg"))){
			return;
		}
	}/*else if(roomNum == 5){
		matchesThreshold = 12;
		Point2f pointCharactor1 = getPoint("character\\left.jpg");
		if(pointCharactor1.x + 1.0 <= 1e-5 && pointCharactor1.y + 1.0 <= 1e-5){
			return;
		}
		if(isExist(getPoint("belowDoor.jpg"))){
			return;
		}
		matchesThreshold = 20;
	}*/
	
	if(roomNum == 1){
		Point2f targetPoint = Point2f(135.959f, 61.333f);
		Point2f signPoint = getPoint("sign1.jpg");
		Point2f pointCharactor= getPoint("character\\left.jpg");
		if((pointCharactor.x + 1.0 <= 1e-5 && pointCharactor.y + 1.0 <= 1e-5) || (signPoint.x + 1.0 <= 1e-5 && signPoint.y + 1.0 <= 1e-5)){
			return;
		}
		targetPoint += signPoint;
		keyMemory_adjustPosition[roomNum] = targetPoint - pointCharactor;
	}
	if(runNum <= 2 && roomNum != 1){
		Point2f targetPoint;
		switch (roomNum)
		{
		case 2:
			targetPoint = Point2f(82.2047f, -42.9638f);
			break;
		case 3:
			targetPoint = Point2f(80.316f, -14.5975f);
			break;
		case 6:
			targetPoint = Point2f(81.944f, -55.8724f);
			break;
		default:
			break;
		}
		Point2f pointCharactor;
		Point2f signPoint;
		/*if(roomNum == 1){
			signPoint = getPoint("sign1.jpg");
			pointCharactor= getPoint("character\\left.jpg");
		}else */
		if(roomNum == 2 || roomNum == 3 || roomNum == 6){
			signPoint = getPoint("leftCloseDoor.jpg");
			pointCharactor= getPoint("character\\left.jpg");
		}/*else{
			Sleep(1000);
		}*/
		if((pointCharactor.x + 1.0 <= 1e-5 && pointCharactor.y + 1.0 <= 1e-5) || (signPoint.x + 1.0 <= 1e-5 && signPoint.y + 1.0 <= 1e-5)){
				return;
		}
		targetPoint += signPoint;
		keyMemory_adjustPosition[roomNum] = targetPoint - pointCharactor;
	}
	if(roomNum == 1 || roomNum == 2 || roomNum == 3 || roomNum == 6){
		runAs(keyMemory_adjustPosition[roomNum], 7.0f, 5.5f);
	}
	SYSTEMTIME st;
	GetLocalTime(&st);
	originTime = st.wHour*60*60*1000 + st.wMinute*60*1000 + st.wSecond*1000 + st.wMilliseconds;
	keyPress();
	sleepTime();
	isExistMonster();
	walkToDoor();
	roomNum++;
	if(roomNum > 7){
		onceAgain();
	}
	cout << "------------------------------" << endl;
	//interval(500);
}

double light(IplImage * image)  
{  
    IplImage * gray = cvCreateImage(cvGetSize(image), image->depth, 1);  
    cvCvtColor(image, gray, CV_BGR2GRAY);    
    double sum = 0;    
    double avg = 0;  
    CvScalar scalar;  
    int ls[256];  
    for(int i=0; i<256; i++)  
        ls[i]=0;  
    for(int i=0;i<gray->height;i++)    
    {    
        for(int j=0;j<gray->width;j++)    
        {    
            scalar = cvGet2D(gray, i, j);  
            sum += (scalar.val[0] - 128);  
            int x= (int)scalar.val[0];    
             ls[x]++;    
        }    
    }    
    avg = sum/(gray->height * gray->width);    
    double total = 0;  
    double mean = 0;  
    for(int i=0;i<256;i++)    
    {    
        total += abs(i-128-avg)* ls[i];    
    }    
    mean = total/(gray->height * gray->width);  
    double cast = abs(avg/mean);    
    //printf("light: %f\n", avg);  
   /* if(cast>1)  
    {  
        if(avg>0)  
            printf("light\n");  
        else printf("dark\n");  
    }  */
	cvReleaseImage(&gray);
	return avg;
}  

//void main(){
//	cout << "start" << endl;
//	init();
//	DD_init();
//	//Sleep(5000);
//	long sleepTime = 50;
//	while(isRunning){
//		if(isQuit()) return;
//		if(isPause()) system("pause");
//		if(isRestart()){
//			runNum = 1;
//			cout << "即将重新开始" << endl;
//			system("pause");
//			continue;
//		}
//		Sleep(sleepTime);
//		cout << "正在查找 " << roomNum << endl;
//		//gameManager();
//		gameManager1();
//	}
//}