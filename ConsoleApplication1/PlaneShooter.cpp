#include "stdafx.h"
#include "PlaneShooter.h"
#include "math.h"
#include "Serial.h"

int rad2deg(double angle)
{
	return (int)(angle * 180 / 3.1415);
}

PlaneShooter::PlaneShooter(void)
{
}


PlaneShooter::~PlaneShooter(void)
{
	delete serial;
}

void PlaneShooter::shootXY(int x, int y, int imgWidth, int imgHeight)
{
	int centerX = imgWidth / 2;
	int centerY = imgHeight / 2;
	int tsidudAngle = (int)rad2deg(atan((double)(x - centerX) / (double)pixelDistance));
	int ilrudAngle = (int)rad2deg(atan((double)(y - centerY) / (double)pixelDistance));
	char fireCommand[32];
	int realAngleX = x0 - tsidudAngle;
	int realAngleY = y0 + ilrudAngle;
	printf ("got %d,%d firing to xAngle = %d, yAngle= %d\n", x, y, realAngleX, realAngleY);
	sprintf_s(fireCommand, "%d,%d,0\n", realAngleX, realAngleY);
	serial->WriteData(fireCommand, strlen(fireCommand));
}
