#include<Windows.h>
#include <iostream>

using namespace std;

void DD_init();
void mouseMove(int, int);
void mousePress();
void KeyDown(int);
void KeyUp(int);
void KeyPress(int);
void KeyPress(int, long);
void interval(long = 50);
short getAsyncKeyState(int);
void mouseMoveAndPress(int, int);