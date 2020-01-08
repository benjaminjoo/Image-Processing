#pragma warning(disable : 4996)

#include "WebCam.h"
#include "Honeycomb.h"

#include <iostream>
#include <ctime>

WebCam::WebCam(int w, int h, Uint32 c):
	width(w), height(h), backgroundColour(c)
{
	Screen		= std::make_shared<Canvas>(width, height, 1.0f, "WebCam");
	Controls	= std::make_shared<EventHandler>(0.0f, 0.0f, 0.0f, 1.0f);	
	Stains		= std::make_shared<Texture>("Assets/stain.jpg");
	Screen->setClearColour(backgroundColour);

	nCameras = setupESCAPI();
	if (nCameras == 0)
		std::cout << "Failed setting up camera..." << std::endl;

	capture.mWidth = width;
	capture.mHeight = height;
	capture.mTargetBuf = new int[width * height];
	oldFrame = new int[width * height];
	newFrame = new int[width * height];
	clearFrame(oldFrame, width, height, 0xffffffff);
	clearFrame(newFrame, width, height, 0xffffffff);

	if (initCapture(0, &capture) == 0)
		std::cout << "Failed initializing capture..." << std::endl;

	srand(unsigned int(time(NULL)));
}


WebCam::~WebCam()
{
	delete[] oldFrame;
	delete[] newFrame;
}


bool WebCam::isRunning()
{
	return isOn;
}


void WebCam::projectUnaltered()
{
	Screen->clear();

	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
		{
			colour32 pixelColour;
			pixelColour.argb = capture.mTargetBuf[j * width + (width -i)];
			Screen->putPixel(i, j, pixelColour.argb);
		}					
	
	Screen->update();
}


void WebCam::projectCircular(std::vector<int3> circles)
{
	Screen->clear();
	
	for (int3& c : circles)
	{
		int x, y, rad;
		x = c.a;
		y = c.b;
		rad = c.c;

		for (int j = 0; j < height; j++)
			for (int i = 0; i < width; i++)
				if (i >= x - rad && i <= x + rad && j >= y - rad && j <= y + rad)
					if ((i - x) * (i - x) + (j - y) * (j - y) <= rad * rad)
					{
						double xRelative = (static_cast<double>(i) - static_cast<double>(x)) / static_cast<double>(rad);
						double yRelative = (static_cast<double>(j) - static_cast<double>(y)) / static_cast<double>(rad);

						int sampleX = static_cast<int>(width / 2 + (height / 2) * xRelative);
						int sampleY = static_cast<int>(height / 2 + (height / 2) * yRelative);
						int pixelIndex = sampleY * width + (width - sampleX);

						colour32 pixelColour;
						pixelColour.argb = capture.mTargetBuf[pixelIndex];

						Screen->putPixel(i, j, pixelColour.argb);
					}
	}

	Screen->update();
}


void WebCam::projectPattern(bool* pattern, int w, int h, int pacingH, int pacingV)
{
	Screen->clear();

	int patternW = w;
	int patternH = h;

	int pacingX = pacingH;
	int pacingY = pacingV;

	int numX = width / pacingX + 3;
	int numY = height / pacingY + 5;

	int remX = (width - numX * pacingX) / 2;
	int remY = (height - numY * pacingY) / 2;

	for (int j = 0; j < numY; j++)
		for (int i = 0; i < numX; i++)
		{
			colour32 finalColour;
			colour32 finalPixel;
			colour32 colourSample;

			int xBase, yBase;
			if (isEven(j))
				xBase = remX + i * pacingX - (patternW - 1) / 2;
			else
				xBase = remX + (static_cast<double>(i) - 0.5f) * pacingX - (patternW - 1) / 2;
			yBase = remY + (static_cast<double>(j) - 0.5f) * pacingY - (patternH - 1) / 2;

			int nSample = 0;
			int rAccu = 0;
			int gAccu = 0;
			int bAccu = 0;
			unsigned char rr = 0;
			unsigned char gg = 0;
			unsigned char bb = 0;

			for (int xx = 0; xx < patternW; xx++)
				for (int yy = 0; yy < patternH; yy++)
				{
					int pixelIndex = yy * patternW + xx;
					if (pattern[pixelIndex])
					{
						int x = xBase + xx;
						int y = yBase + yy;
						if (x >= 0 && x < width && y >= 0 && y < height)
						{
							colourSample.argb = capture.mTargetBuf[y * width + (width - x)];
							rAccu += colourSample.c[2];
							gAccu += colourSample.c[1];
							bAccu += colourSample.c[0];
							nSample++;
						}
					}
				}

			if (nSample)
			{
				rr = static_cast<int>(static_cast<double>(rAccu) / nSample);
				gg = static_cast<int>(static_cast<double>(gAccu) / nSample);
				bb = static_cast<int>(static_cast<double>(bAccu) / nSample);

				finalColour.c[3] = 0;
				finalColour.c[2] = rr;
				finalColour.c[1] = gg;
				finalColour.c[0] = bb;
			}	

			finalPixel = getGrayScalePixelDiscreet(finalColour, 16);

			for (int xx = 0; xx < patternW; xx++)
				for (int yy = 0; yy < patternH; yy++)
				{
					int pixelIndex = yy * patternW + xx;
					if (pattern[pixelIndex])
					{
						int x = xBase + xx;
						int y = yBase + yy;
						if (x >= 0 && x < width && y >= 0 && y < height)
							Screen->putPixel(x, y, finalPixel.argb);
					}
				}
		}

	Screen->update();
}


void WebCam::projectContrast()
{
	Screen->clear();

	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
		{
			colour32 currentPixel;
			currentPixel.argb = capture.mTargetBuf[j * width + i];
			if (currentPixel.c[2] + currentPixel.c[1] + currentPixel.c[0] > brightness)
				Screen->putPixel(i, j, backgroundColour);
			else
				Screen->putPixel(i, j, 0x00000000);
		}

	Screen->displayString("BRIGHTNESS:", 1, 4, lateral::LEFT, 0x00ff0000);
	Screen->displayInt(brightness, 8, 14, 4, lateral::LEFT, 0x00ff0000);

	Screen->update();
}


void WebCam::projectMovement()
{
	copyFrame(newFrame, capture.mTargetBuf, width, height);

	Screen->clear();

	int changingPixels = 0;

	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
		{
			int id = j * width + i;
			colour32 oldColour, newColour;
			oldColour.argb = oldFrame[id];
			newColour.argb = newFrame[id];

			colour32 delta = subtractColour(oldColour, newColour);
			if (delta.c[2] + delta.c[1] + delta.c[0] > 100)
				changingPixels++;
			if (delta.c[2] + delta.c[1] + delta.c[0] > 25)
			{
				delta.c[0] = 0;
				Screen->putPixel(i, j, delta.argb);
			}				
			else
				Screen->putPixel(i, j, 0x00000000);
		}

	Screen->displayString("SENSITIVITY:", 1, 4, lateral::LEFT, 0x000000ff);
	Screen->displayInt(motionTriggerSensitivity, 8, 14, 4, lateral::LEFT, 0x000000ff);

	if (changingPixels > motionTriggerSensitivity)
		std::cout << "ACHTUNG!!! " << changingPixels << std::endl;

	Screen->update();

	copyFrame(oldFrame, newFrame, width, height);
}


void WebCam::projectNoiseReduction()
{
	Screen->clear();

	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
		{
			int id = j * width + (width - i);

			colour32 oldColour, newColour, finalColour;
			oldColour.argb = oldFrame[id];
			newColour.argb = capture.mTargetBuf[id];

			double oldG = getGrayScale(oldColour);
			double newG = getGrayScale(newColour);
			double delta = (newG - oldG) * 0.5f;
			double finalG = oldG + delta;
			if (finalG > 1.0f)
				finalG = 1.0f;
			if (finalG < 0.0f)
				finalG = 0.0f;

			finalColour = getGrayScalePixel(finalG);
			oldFrame[id] = finalColour.argb;
			Screen->putPixel(i, j, finalColour.argb);
		}



	Screen->update();
}


void WebCam::projectPatternNoiseReduction(bool* pattern, int w, int h, int pacingH, int pacingV)
{
	Screen->clear();

	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
		{
			int id = j * width + (width - i);

			colour32 oldColour, newColour, finalColour;
			oldColour.argb = oldFrame[id];
			newColour.argb = capture.mTargetBuf[id];

			double oldG = getGrayScale(oldColour);
			double newG = getGrayScale(newColour);
			double delta = (newG - oldG) * 0.1f;
			double finalG = oldG + delta;
			if (finalG > 1.0f)
				finalG = 1.0f;
			if (finalG < 0.0f)
				finalG = 0.0f;

			finalColour = getGrayScalePixel(finalG);
			oldFrame[id] = finalColour.argb;
		}

	int patternW = w;
	int patternH = h;

	int pacingX = pacingH;
	int pacingY = pacingV;

	int numX = width / pacingX + 3;
	int numY = height / pacingY + 5;

	int remX = (width - numX * pacingX) / 2;
	int remY = (height - numY * pacingY) / 2;

	for (int j = 0; j < numY; j++)
		for (int i = 0; i < numX; i++)
		{
			colour32 finalPixel;
			colour32 colourSample;

			int xBase, yBase;
			double xCentre, yCentre;

			if (isEven(j))
			{
				xBase = remX + i * pacingX - (patternW - 1) / 2;
				xCentre = static_cast<double>(remX + i * pacingX) / static_cast<double>(width);
			}
			else
			{
				xBase = remX + (static_cast<double>(i) - 0.5f) * pacingX - (patternW - 1) / 2;
				xCentre = (static_cast<double>(remX) + (static_cast<double>(i) - 0.5f) * pacingX) / static_cast<double>(width);
			}
			yBase = remY + (static_cast<double>(j) - 0.5f) * pacingY - (patternH - 1) / 2;
			yCentre = (static_cast<double>(remY) + (static_cast<double>(j) - 0.5f) * pacingY) / static_cast<double>(height);

			if (xCentre < 0.0f)
				xCentre = 0.0f;
			if (yCentre < 0.0f)
				yCentre = 0.0f;

			colour32 stainTest;
			stainTest.argb = Stains->getPixel(xCentre, yCentre);
			bool stained = false;
			if ((stainTest.c[2] < 127) && (stainTest.c[1] < 127) && (stainTest.c[0] > 0))
				stained = true;

			int nSample = 0;
			int rAccu = 0;
			int gAccu = 0;
			int bAccu = 0;
			unsigned char rr = 0;
			unsigned char gg = 0;
			unsigned char bb = 0;

			if (!stained)
				for (int xx = 0; xx < patternW; xx++)
					for (int yy = 0; yy < patternH; yy++)
					{
						int pixelIndex = yy * patternW + xx;
						if (pattern[pixelIndex])
						{
							int x = xBase + xx;
							int y = yBase + yy;
							if (x >= 0 && x < width && y >= 0 && y < height)
							{
								colourSample.argb = oldFrame[y * width + (width - x)];
								rAccu += colourSample.c[2];
								gAccu += colourSample.c[1];
								bAccu += colourSample.c[0];
								nSample++;
							}
						}
					}

			if (nSample)
			{
				rr = static_cast<int>(static_cast<double>(rAccu) / nSample);
				gg = static_cast<int>(static_cast<double>(gAccu) / nSample);
				bb = static_cast<int>(static_cast<double>(bAccu) / nSample);

				finalPixel.c[3] = 0;
				finalPixel.c[2] = rr;
				finalPixel.c[1] = gg;
				finalPixel.c[0] = bb;
			}

			for (int xx = 0; xx < patternW; xx++)
				for (int yy = 0; yy < patternH; yy++)
				{
					int pixelIndex = yy * patternW + xx;
					if (pattern[pixelIndex])
					{
						int x = xBase + xx;
						int y = yBase + yy;
						if (x >= 0 && x < width && y >= 0 && y < height)
							Screen->putPixel(x, y, finalPixel.argb);
					}
				}
		}

	Screen->update();
}


void WebCam::projectMandelbrot(double scale, double offsetX, double offsetY)
{
	//Displays Mandelbrot Set based on pseudocode found on Wikipedia

	Screen->clear();

	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
		{
			double screenUnit = static_cast<double>(height) * 0.5f;
			//double centreX = static_cast<double>(width) / 7.0f * 5.0f;
			//double centreY = static_cast<double>(height) * 0.5f;
			double centreX = 0.0f;
			double centreY = 0.0f;

			double x_0 = ((static_cast<double>(i) - centreX) / screenUnit + screenOffsetX) * scale;
			double y_0 = ((static_cast<double>(j) - centreY) / screenUnit + screenOffsetY) * scale;

			double x = 0.0f;
			double y = 0.0f;

			int iter = 0;
			int maxIter = 256;

			double rSquare = 0.0f;
			double iSquare = 0.0f;
			double zSquare = 0.0f;

			while (rSquare + iSquare <= 4.0 && iter < maxIter)
			{
				x = rSquare - iSquare + x_0;
				y = zSquare - rSquare - iSquare + y_0;
				rSquare = x * x;
				iSquare = y * y;
				zSquare = (x + y) * (x + y);
				iter++;
			}

			//while (x * x + y * y <= 4.0f && iter < maxIter)
			//{
			//	double xTemp = x * x - y * y + x_0;
			//	y = 2 * x * y + y_0;
			//	x = xTemp;
			//	iter++;
			//}

			colour32 finalPixel;

			finalPixel.c[3] = 0;
			finalPixel.c[2] = 0;
			finalPixel.c[1] = 0;
			finalPixel.c[0] = iter;

			Screen->putPixel(i, j, finalPixel.argb);
		}

	Screen->update();
}


void WebCam::clearFrame(int* frame, int w, int h, int c)
{
	for (int* i = frame; i < frame + w * h; i++)
		*i = c;
}


void WebCam::copyFrame(int* frameA, int* frameB, int w, int h)
{
	for (int i = 0; i < w * h; i++)
		frameA[i] = frameB[i];
}


void WebCam::addViewCircle(int3 c)
{
	viewCircles.push_back(c);
}


void WebCam::update()
{
	Controls->HandleUserEvents();

	isOn = Controls->isRunning();
	brightness = Controls->brightness;
	motionTriggerSensitivity = Controls->mtSens;
	screenScale = Controls->sensitivity;
	screenOffsetX += Controls->strafe;
	screenOffsetY += Controls->move;

	doCapture(0); while (isCaptureDone(0) == 0) {}

	switch (Controls->styleFlag)
	{
		case 0:
			projectUnaltered();
			break;
		case 1:
			projectContrast();
			break;
		case 2:
			projectNoiseReduction();
			break;
		case 3:
			//projectPatternNoiseReduction(honeycomb_tiny, 9, 8, 14, 4);
			projectPattern(honeycomb_tiny, 9, 8, 14, 4);
			//projectPattern(honeycomb_small, 18, 16, 28, 8);
			//projectPattern(circle, 16, 16, 32, 8);
			//projectPattern(eye, 22, 16, 28, 8);
			//projectPattern(brick, 8, 32, 16, 16);
			//projectPattern(brick_tiny, 4, 16, 8, 8);
			break;
		case 4:
			projectMovement();
			break;
		case 5:
			{
				if (viewCircles.size())
					projectCircular(viewCircles);
			}
			break;
		case 6:
			projectMandelbrot(screenScale, screenOffsetX, screenOffsetY);
			break;
		default:
			projectUnaltered();
			break;
	}

	Controls->ceaseMotion();
}
