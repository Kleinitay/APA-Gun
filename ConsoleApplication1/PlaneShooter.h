#pragma once
#include "Serial.h"



class PlaneShooter
{
public:
	int pixelDistance;
	Serial *serial;
	int x0, y0;
	PlaneShooter(void);
	PlaneShooter(int _pixelDistance, int _x0, int _y0)
	{
		pixelDistance = _pixelDistance;
		x0 = _x0;
		y0 = _y0;
		serial = new Serial(L"\\\\.\\COM3");
	}

	void shootXY(int x, int y, int imgWidth, int imgHeight);
	~PlaneShooter(void);
};



