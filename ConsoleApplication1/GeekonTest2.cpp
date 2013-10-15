// GeekonTest2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Serial.h"
#include <stdio.h>
#include <tchar.h>
#include <string>
#include "PlaneShooter.h"

LPCWSTR convertToWChar(const char *str)
{
	int len = strlen(str) + 1;
	wchar_t *w_msg = new wchar_t[len];
	memset(w_msg, 0, len);
	MultiByteToWideChar(CP_ACP, NULL, str, -1, w_msg, len);
	return (LPCWSTR)w_msg;
}

int _tmain(int argc, _TCHAR* argv[])
{
	PlaneShooter p(640, 97,85);
	//p.shootXY(320, 240, 640, 480);
	p.shootXY(150, 240, 640, 480);
	Sleep(1000);
	p.shootXY(450, 150, 640, 480);
	Sleep(1000);
	p.shootXY(350, 240, 640, 480);
	//LPCWSTR port = L"\\\\.\\COM3";//convertToWChar("COM3");
	//Serial serial(port);
	//serial.WriteData("150,60,0\n", 10);
	return 0;
}
