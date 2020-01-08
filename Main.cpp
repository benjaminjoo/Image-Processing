#include "WebCam.h"

#include <iostream>

int main(int argc, char** argv)
{
	auto Cam = std::make_shared<WebCam>(1200, 600, 0x00000000);

	int numH = 6;
	int numV = 3;
	int rad = 75;
	int pacingH = 200;
	int pacingV = 200;
	int topLeftX = (1200 - (numH - 1) * pacingH) / 2;
	int topLeftY = (600 - (numV - 1) * pacingV) / 2;

	for (int j = 0; j < numV; j++)
		for (int i = 0; i < numH; i++)
		{
			int x = topLeftX + i * pacingH;
			int y = topLeftY + j * pacingV;
			Cam->addViewCircle({ x, y, rad });
		}

	while (Cam->isRunning())
		Cam->update();

	return 0;
}