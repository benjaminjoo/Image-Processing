#include <iostream>
#include <cstdlib>
#include <ctime>

#include "Canvas.h"
#include "Utilities.h"
#include "Fonts.h"
#include "lodepng.h"


Canvas::Canvas(const int& w, const int& h, const double& s, const std::string& title):
	width(w), height(h), scale(s)
{
	pixelBuffer	= new Uint32[width * height];

	SDL_Init(SDL_INIT_EVERYTHING);

	window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);

	screen = SDL_CreateRenderer(window, -1, 0);

	texture = SDL_CreateTexture(screen, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);

	SDL_ShowCursor(SDL_DISABLE);

	std::cout << "Canvas created succesfully..." << std::endl;
}


Canvas::~Canvas()
{
	SDL_DestroyTexture(texture);

	SDL_DestroyRenderer(screen);

	SDL_DestroyWindow(window);

	delete[] pixelBuffer;

	std::cout << "Canvas destroyed succesfully..." << std::endl;
}


void Canvas::setClearColour(Uint32 col)
{
	clearColour = col;
}


void Canvas::clear()
{
	for (int i = 0; i < width * height; i++) { pixelBuffer[i] = clearColour; }
}


void Canvas::update()
{
	SDL_UpdateTexture(texture, nullptr, pixelBuffer, width * sizeof(Uint32));

	SDL_RenderClear(screen);

	SDL_RenderCopy(screen, texture, nullptr, nullptr);

	SDL_RenderPresent(screen);
}


void Canvas::putPixel(int x, int y, Uint32 colour)
{
	if (x >= 0 && x < width && y >= 0 && y < height)
	{
		pixelBuffer[y * width + x] = colour;
	}
}


void Canvas::putPixel(int n, Uint32 colour)
{
	if (n >= 0 && n < width * height)
		pixelBuffer[n] = colour;
}


int Canvas::getWidth() { return width; }


int Canvas::getHeight() { return height; }


double Canvas::getScale() { return scale; }


void Canvas::drawLine(const screenCoord& startP, const screenCoord& endP, const Uint32& colour)
{
	/*No clipping takes place in this method, if either startP or endP
	are off screen, line is discarded entirely. It is assumed that all
	lines form sides of triangles already clipped to the view frustum*/
	
	/*Check if line endpoints differ, and if so, see how much they do*/
	bool diffX = (endP.x - startP.x == 0) ? false : true;
	bool diffY = (endP.y - startP.y == 0) ? false : true;
	int deltaX = endP.x - startP.x;
	int deltaY = endP.y - startP.y;

	int x1, y1, x2, y2;

	if (!diffX && !diffY)
	{
		/*No extent - draw single pixel*/
		int xCurrent = startP.x;
		int yCurrent = startP.y;
		if ((xCurrent >= 0 && xCurrent < width) && (yCurrent >= 0 && yCurrent < height))
		{
			pixelBuffer[yCurrent * width + xCurrent] = colour;
		}
	}
	if (!diffX && diffY)
	{
		/*No 'X' extent - draw vertical columns of pixels*/
		if (startP.y < endP.y)
		{
			x1 = startP.x;	y1 = startP.y;
			x2 = endP.x;	y2 = endP.y;
		}
		else
		{
			x1 = endP.x;	y1 = endP.y;
			x2 = startP.x;	y2 = startP.y;
		}
		for (int xCurrent = x1, yCurrent = y1; yCurrent < y2; yCurrent++)
		{
			if ((xCurrent >= 0 && xCurrent < width) && (yCurrent >= 0 && yCurrent < height))
			{
				pixelBuffer[yCurrent * width + xCurrent] = colour;
			}
		}
	}
	if (!diffY && diffX)
	{
		/*No 'Y' extent - draw horizontal row of pixels*/
		if (startP.x < endP.x)
		{
			x1 = startP.x;	y1 = startP.y;
			x2 = endP.x;	y2 = endP.y;
		}
		else
		{
			x1 = endP.x;	y1 = endP.y;
			x2 = startP.x;	y2 = startP.y;
		}
		for (int xCurrent = x1, yCurrent = y1; xCurrent < x2; xCurrent++)
		{
			if ((xCurrent >= 0 && xCurrent < width) && (yCurrent >= 0 && yCurrent < height))
			{
				pixelBuffer[yCurrent * width + xCurrent] = colour;
			}
		}
	}
	if (diffX && diffY)
	{
		/*Line extends both in the 'X' and 'Y' directions*/
		if (abs(deltaX) == abs(deltaY))
		{
			/*Line extends equally in both directions*/
			int stepY = 0;
			if (startP.x < endP.x)
			{
				x1 = startP.x;	y1 = startP.y;
				x2 = endP.x;	y2 = endP.y;
				stepY = (startP.y < endP.y) ? 1 : -1;
			}
			else
			{
				x1 = endP.x;	y1 = endP.y;
				x2 = startP.x;	y2 = startP.y;
				stepY = (startP.y < endP.y) ? -1 : 1;
			}
			for (int xCurrent = x1, yCurrent = y1; xCurrent < x2; xCurrent++, yCurrent += stepY)
			{
				if ((xCurrent >= 0 && xCurrent < width) && (yCurrent >= 0 && yCurrent < height))
				{
					pixelBuffer[yCurrent * width + xCurrent] = colour;
				}
			}
		}
		else
		{
			/*Generic line*/
			if (abs(deltaX) >= abs(deltaY))
			{
				/*Line is 'X'-major*/
				int stepY = 0, xCount = 0, yCount = 0;
				if (startP.x < endP.x)
				{
					x1 = startP.x;	y1 = startP.y;
					x2 = endP.x;	y2 = endP.y;
					stepY = (startP.y < endP.y) ? 1 : -1;
				}
				else
				{
					x1 = endP.x;	y1 = endP.y;
					x2 = startP.x;	y2 = startP.y;
					stepY = (startP.y < endP.y) ? -1 : 1;
				}
				for (int xCurrent = x1; xCurrent < x2; xCurrent++, xCount++)
				{
					int yCurrent = y1 + yCount;
					if ((xCurrent >= 0 && xCurrent < width) && (yCurrent >= 0 && yCurrent < height))
					{
						pixelBuffer[yCurrent * width + xCurrent] = colour;
					}
					float yIdeal = ((float)deltaY / (float)deltaX) * xCount;
					if (abs(yIdeal - yCount) > 0.5f) { yCount += stepY; }
				}
			}
			else
			{
				/*Line is 'Y'-major*/
				int stepX = 0, xCount = 0, yCount = 0;
				if (startP.y < endP.y)
				{
					x1 = startP.x;	y1 = startP.y;
					x2 = endP.x;	y2 = endP.y;
					stepX = (startP.x < endP.x) ? 1 : -1;
				}
				else
				{
					x1 = endP.x;	y1 = endP.y;
					x2 = startP.x;	y2 = startP.y;
					stepX = (startP.x < endP.x) ? -1 : 1;
				}
				for (int yCurrent = y1; yCurrent < y2; yCurrent++, yCount++)
				{
					int xCurrent = x1 + xCount;
					if ((xCurrent >= 0 && xCurrent < width) && (yCurrent >= 0 && yCurrent < height))
					{
						pixelBuffer[yCurrent * width + xCurrent] = colour;
					}
					float xIdeal = ((float)deltaX / (float)deltaY) * yCount;
					if (abs(xIdeal - xCount) > 0.5f) { xCount += stepX; }
				}
			}
		}
	}
}


void Canvas::drawBoundingBox(boundingBoxScreen BB, const Uint32& colour)
{
	BB.clipToScreen(width, height);

	this->drawLine(BB.topLeft, screenCoord(BB.bottomRight.x, BB.topLeft.y), 255);

	this->drawLine(screenCoord(BB.topLeft.x, BB.bottomRight.y), BB.bottomRight, 255);

	this->drawLine(BB.topLeft, screenCoord(BB.topLeft.x, BB.bottomRight.y), 255);

	this->drawLine(screenCoord(BB.bottomRight.x, BB.topLeft.y), BB.bottomRight, 255);
}


void Canvas::drawCircle(const screenCoord& centreP, const int& radius, const Uint32& colour)
{
	boundingBoxScreen bb(centreP.x - radius, centreP.y - radius, centreP.x + radius, centreP.y + radius);
	bb.clipToScreen(width, height);

	for (int j = bb.topLeft.y; j <= bb.bottomRight.y; j++)
	{
		for (int i = bb.topLeft.x; i <= bb.bottomRight.x; i++)
		{
			int pos = (i - centreP.x) * (i - centreP.x) + (j - centreP.y) * (j - centreP.y);
			//if (pos < (radius * radius))
			//	pixelBuffer[j * width + i] = colour;
			int brightness = 255 - (int)((double)pos / ((double)radius * (double)radius) * 255);
			if (pos <= (radius * radius))
			{
				pixelBuffer[j * width + i] = argbColour(0, brightness, brightness, 255);
			}
		}
	}
}


void Canvas::drawRectangle(const screenCoord& topLeft, const screenCoord& bottomRight, const Uint32& colour)
{
	this->drawLine(screenCoord(topLeft.x, topLeft.y), screenCoord(bottomRight.x, topLeft.y), colour);
	this->drawLine(screenCoord(topLeft.x, bottomRight.y), screenCoord(bottomRight.x, bottomRight.y), colour);
	this->drawLine(screenCoord(topLeft.x, topLeft.y), screenCoord(topLeft.x, bottomRight.y), colour);
	this->drawLine(screenCoord(bottomRight.x, topLeft.y), screenCoord(bottomRight.x, bottomRight.y), colour);
}


void Canvas::solidFillTriangle(const screenCoord& a, const screenCoord& b, const screenCoord& c, const Uint32& colour)
{
	screenCoord pt[3] = { a, b, c };
	int yMin = pt[0].y;
	for (int i = 1; i < 3; i++) { if (pt[i].y < yMin) { yMin = pt[i].y; } }
	int yMax = pt[0].y;
	for (int i = 1; i < 3; i++) { if (pt[i].y > yMax) { yMax = pt[i].y; } }

	int wd = 0;
	int dx, dy;
	int lineEnd[2] = { 0, 0 };
	double xx, yy;
	int endIndex;
	int startX, endX;

	for (int hg = yMin; hg < yMax; hg++)
	{
		endIndex = 0;
		//Side A-B:
		if ((a.y <= hg && b.y > hg) || (b.y <= hg && a.y > hg))
		{
			dx = b.x - a.x; dy = b.y -a.y;
			yy = (double)hg - (double)a.y; xx = dx * (yy / dy);
			wd = a.x + (int)xx;
			if (endIndex < 2) { lineEnd[endIndex++] = wd; }
		}
		//Side B-C:
		if ((b.y <= hg && c.y > hg) || (c.y <= hg && b.y > hg))
		{
			dx = c.x - b.x; dy = c.y - b.y;
			yy = (double)hg - (double)b.y; xx = dx * (yy / dy);
			wd = b.x + (int)xx;
			if (endIndex < 2) { lineEnd[endIndex++] = wd; }
		}
		//Side C-A:
		if ((c.y <= hg && a.y > hg) || (a.y <= hg && c.y > hg))
		{
			dx = a.x - c.x; dy = a.y - c.y;
			yy = (double)hg - (double)c.y; xx = dx * (yy / dy);
			wd = c.x + (int)xx;
			if (endIndex < 2) { lineEnd[endIndex++] = wd; }
		}
		if (endIndex == 2)
		{
			if (lineEnd[0] <= lineEnd[1])
			{
				startX = lineEnd[0];
				endX = lineEnd[1];
			}
			else
			{
				startX = lineEnd[1];
				endX = lineEnd[0];
			}
			int span = abs(endX - startX + 1);
			for (int i = startX; i < endX + 1; i++)
			{
				if ((i >= 0 && i < width) && (hg >= 0 && hg < height))
				{
					pixelBuffer[hg * width + i] = colour;
				}
			}
		}
	}
}


void Canvas::solidFillConvexPoly(unsigned int n, const screenCoord* P, const Uint32& colour)
{
	for (unsigned int i = 0; i < n - 2; i++)
	{
		this->solidFillTriangle(P[0], P[i + 1], P[i + 2], colour);
	}
}


void Canvas::renderTriangle(const triangle2& t, vect2 A, vect2 B, vect2 C, const double& scale, Texture* texture)
{
	screenCoord pt[3] = { t.a, t.b, t.c };
	int yMin, yMax;
	yMin = GetYMin3(pt);
	yMax = GetYMax3(pt);

	int wd = 0;
	int dx, dy;
	double xx, yy;
	int lineEnd[2] = { 0, 0 };

	int endIndex;
	int startX, endX;

	vect2 U = (C - B).norm();
	vect2 V = (A - B).norm();
	double uLength = (C - B).len();
	double vLength = (A - B).len();

	Uint32 finalPixel;

	for (int hg = yMin; hg < yMax; hg++)
	{
		endIndex = 0;
		//Side A-B:
		if ((t.a.y <= hg && t.b.y > hg) || (t.b.y <= hg && t.a.y > hg))
		{
			dx = t.b.x - t.a.x; dy = t.b.y - t.a.y;
			yy = (double)hg - (double)t.a.y; xx = dx * (yy / dy);
			wd = t.a.x + (int)xx;
			if (endIndex < 2)
			{
				lineEnd[endIndex++] = wd;
			}
		}
		//Side B-C:
		if ((t.b.y <= hg && t.c.y > hg) || (t.c.y <= hg && t.b.y > hg))
		{
			dx = t.c.x - t.b.x; dy = t.c.y - t.b.y;
			yy = (double)hg - (double)t.b.y; xx = dx * (yy / dy);
			wd = t.b.x + (int)xx;
			if (endIndex < 2)
			{
				lineEnd[endIndex++] = wd;
			}
		}
		//Side C-A:
		if ((t.c.y <= hg && t.a.y > hg) || (t.a.y <= hg && t.c.y > hg))
		{
			dx = t.a.x - t.c.x; dy = t.a.y - t.c.y;
			yy = (double)hg - (double)t.c.y; xx = dx * (yy / dy);
			wd = t.c.x + (int)xx;
			if (endIndex < 2)
			{
				lineEnd[endIndex++] = wd;
			}
		}
		if (endIndex == 2)
		{
			if (lineEnd[0] <= lineEnd[1])
			{
				startX = lineEnd[0];
				endX = lineEnd[1];
			}
			else
			{
				startX = lineEnd[1];
				endX = lineEnd[0];
			}
			int span = abs(endX - startX + 1);

			for (int i = startX; i < endX + 1; i++)
			{
				if ((i >= 0 && i < width) && (hg >= 0 && hg < height))
				{
					vect2 currentPoint((double)i / scale, (double)hg / scale);
					
					vect2 P = (currentPoint - B);
					double uCoord = abs((P * U) / uLength);
					double vCoord = abs((P * V) / vLength);

					finalPixel = texture->getPixel(uCoord, vCoord);

					if (finalPixel)
					{
						pixelBuffer[hg * width + i] = finalPixel;
					}				
				}
			}
		}
	}
}


bool* Canvas::GetSingleDigit_8(char letter_No)
{
	bool* currentLetter;

	switch (letter_No)
	{
	case 0:
	case '0':
		currentLetter = number_0;
		break;
	case 1:
	case '1':
		currentLetter = number_1;
		break;
	case 2:
	case '2':
		currentLetter = number_2;
		break;
	case 3:
	case '3':
		currentLetter = number_3;
		break;
	case 4:
	case '4':
		currentLetter = number_4;
		break;
	case 5:
	case '5':
		currentLetter = number_5;
		break;
	case 6:
	case '6':
		currentLetter = number_6;
		break;
	case 7:
	case '7':
		currentLetter = number_7;
		break;
	case 8:
	case '8':
		currentLetter = number_8;
		break;
	case 9:
	case '9':
		currentLetter = number_9;
		break;
	case '.':
		currentLetter = decimal_p;
		break;
	case '%':
		currentLetter = percent_;
		break;
	case 'a':
		currentLetter = letter_a;
		break;
	case 'A':
		currentLetter = letter_A;
		break;
	case 'b':
		currentLetter = letter_b;
		break;
	case 'B':
		currentLetter = letter_B;
		break;
	case 'c':
		currentLetter = letter_c;
		break;
	case 'C':
		currentLetter = letter_C;
		break;
	case 'd':
		currentLetter = letter_d;
		break;
	case 'D':
		currentLetter = letter_D;
		break;
	case 'e':
		currentLetter = letter_e;
		break;
	case 'E':
		currentLetter = letter_E;
		break;
	case 'f':
		currentLetter = letter_f;
		break;
	case 'F':
		currentLetter = letter_F;
		break;
	case 'g':
		currentLetter = letter_g;
		break;
	case 'G':
		currentLetter = letter_G;
		break;
	case 'h':
		currentLetter = letter_h;
		break;
	case 'H':
		currentLetter = letter_H;
		break;
	case 'i':
		currentLetter = letter_i;
		break;
	case 'I':
		currentLetter = letter_I;
		break;
	case 'j':
		currentLetter = letter_j;
		break;
	case 'J':
		currentLetter = letter_J;
		break;
	case 'k':
		currentLetter = letter_k;
		break;
	case 'K':
		currentLetter = letter_K;
		break;
	case 'l':
		currentLetter = letter_l;
		break;
	case 'L':
		currentLetter = letter_L;
		break;
	case 'm':
		currentLetter = letter_m;
		break;
	case 'M':
		currentLetter = letter_M;
		break;
	case 'n':
		currentLetter = letter_n;
		break;
	case 'N':
		currentLetter = letter_N;
		break;
	case 'o':
		currentLetter = letter_o;
		break;
	case 'O':
		currentLetter = letter_O;
		break;
	case 'p':
		currentLetter = letter_p;
		break;
	case 'P':
		currentLetter = letter_P;
		break;
	case 'q':
		currentLetter = letter_q;
		break;
	case 'Q':
		currentLetter = letter_Q;
		break;
	case 'r':
		currentLetter = letter_r;
		break;
	case 'R':
		currentLetter = letter_R;
		break;
	case 's':
		currentLetter = letter_s;
		break;
	case 'S':
		currentLetter = letter_S;
		break;
	case 't':
		currentLetter = letter_t;
		break;
	case 'T':
		currentLetter = letter_T;
		break;
	case 'u':
		currentLetter = letter_u;
		break;
	case 'U':
		currentLetter = letter_U;
		break;
	case 'v':
		currentLetter = letter_v;
		break;
	case 'V':
		currentLetter = letter_V;
		break;
	case 'w':
		currentLetter = letter_w;
		break;
	case 'W':
		currentLetter = letter_W;
		break;
	case 'x':
		currentLetter = letter_x;
		break;
	case 'X':
		currentLetter = letter_X;
		break;
	case 'y':
		currentLetter = letter_y;
		break;
	case 'Y':
		currentLetter = letter_Y;
		break;
	case 'z':
		currentLetter = letter_z;
		break;
	case 'Z':
		currentLetter = letter_Z;
		break;
	default:
		currentLetter = letter__;
		break;
	}

	return currentLetter;
}


void Canvas::displayFloat(double value, int dec, int shiftH, int shiftV, Uint32 colour)
{
	bool isSigned;
	isSigned = value >= 0.0 ? false : true;
	if (value < 0.0) { value = -value; }
	std::shared_ptr<int[]> fract = getFractionals(value, dec);
	int nInt = 0;
	std::shared_ptr<int[]> dInt = getIntegers(value, &nInt);

	int posH = width - 1 - (dec + 1 + shiftH) * fontSize;
	int posV = height - fontSize * shiftV;

	for (int p = 0; p < dec; p++)
	{
		bool* currentMap = this->GetSingleDigit_8((char)(fract[p]));
		for (int j = 0; j < fontSize; j++)
		{
			for (int i = 0; i < fontSize; i++)
			{
				if (currentMap[j * fontSize + i])
				{
					pixelBuffer[(posV + j) * width + posH + i] = colour;
				}
			}
		}
		posH += fontSize;
	}

	posH = width - 1 - (dec + nInt + 2 + shiftH) * fontSize;

	for (int p = 0; p < nInt; p++)
	{
		bool* currentMap = this->GetSingleDigit_8((char)(dInt[p]));
		for (int j = 0; j < fontSize; j++)
		{
			for (int i = 0; i < fontSize; i++)
			{
				if (currentMap[j * fontSize + i])
				{
					pixelBuffer[(posV + j) * width + posH + i] = colour;
				}
			}
		}
		posH += fontSize;
	}

	posH = width - 1 - (dec + 2 + shiftH) * fontSize;

	bool* currentMap = this->GetSingleDigit_8('.');
	for (int j = 0; j < fontSize; j++)
	{
		for (int i = 0; i < fontSize; i++)
		{
			if (currentMap[j * fontSize + i])
			{
				pixelBuffer[(posV + j) * width + posH + i] = colour;
			}
		}
	}

	posH = width - 1 - (dec + nInt + 3 + shiftH) * fontSize;

	if (isSigned)
	{
		bool* currentMap = minus_sign;
		for (int j = 0; j < fontSize; j++)
		{
			for (int i = 0; i < fontSize; i++)
			{
				if (currentMap[j * fontSize + i])
				{
					pixelBuffer[(posV + j) * width + posH + i] = colour;
				}
			}
		}
	}
}


void Canvas::displayInt(int value, int font_size, int shiftH, int shiftV, lateral alignment, Uint32 colour)
{
	int nInt = 0;
	std::shared_ptr<int[]> dInt = getIntegers(value, &nInt);

	int posH, posV;
	if (alignment == lateral::LEFT)
		posH = fontSize * shiftH;
	else
		posH = width - fontSize * shiftH;
	posV = height - fontSize * shiftV;

	for (int p = 0; p < nInt; p++)
	{
		bool* currentMap = this->GetSingleDigit_8((char)(dInt[p]));
		for (int j = 0; j < font_size; j++)
		{
			for (int i = 0; i < font_size; i++)
			{
				if (currentMap[j * font_size + i])
				{
					if(posV + j >= 0 && posV + j < height && posH + i >= 0 && posH + i < width)
						pixelBuffer[(posV + j) * width + posH + i] = colour;
				}
			}
		}
		posH += font_size;
	}
}


void Canvas::displayString(const char* text, int shiftH, int shiftV, lateral alignment, Uint32 col)
{
	int nChar = 0;
	while (text[nChar] != '\0') { nChar++; }

	int posH, posV;
	if (alignment == lateral::LEFT)
		posH = fontSize * shiftH;
	else
		posH = width - fontSize * shiftH;
	posV = height - fontSize * shiftV;

	for (int c = 0; c < nChar; c++)
	{
		bool* currentMap = this->GetSingleDigit_8(text[c]);
		for (int j = 0; j < fontSize; j++)
		{
			for (int i = 0; i < fontSize; i++)
			{
				if (currentMap[j * fontSize + i])
				{
					pixelBuffer[(posV + j) * width + posH + i] = col;
				}
			}
		}
		posH += fontSize;
	}
}


bool Canvas::checkPolygonForSplitting(int n, vect2* V, edge e)
{
	bool needsSplitting = false;
	vect2 currentStart, currentEnd;
	double sStart, sEnd;
	vect2 P, Q;
	for (int i = 0; i < n - 1; i++)
	{
		P.x = V[i].x;
		P.y = V[i].y;
		Q.x = V[i + 1].x;
		Q.y = V[i + 1].y;
		currentStart	= P - e.startP;
		currentEnd		= Q - e.startP;
		sStart	= currentStart	* e.normal;
		sEnd	= currentEnd	* e.normal;
		if (sign(sStart) && sign(sEnd))
		{
			if (sign(sStart) != sign(sEnd)) { needsSplitting = true; }
		}
	}
	return needsSplitting;
}


bool Canvas::iSect2dLine(vect2 a, vect2 b, edge e, vect2* result)
{
	vect2 ap = a - e.startP;
	vect2 bp = b - e.startP;

	double sA = ap * e.normal;
	double sB = bp * e.normal;

	double t;

	if (sign(sA) != sign(sB))
	{
		vect2 d = b - a;
		double dist = d * e.normal;

		if (sA < 0.0)
		{
			if (dist)
			{
				t = (dist - sB) / dist;

				result->x = a.x + t * (b.x - a.x);
				result->y = a.y + t * (b.y - a.y);
			}
		}
		if (sB < 0.0)
		{
			if (dist)
			{
				t = (-dist - sA) / dist;

				result->x = b.x - t * (a.x - b.x);
				result->y = b.y - t * (a.y - b.y);
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}


void Canvas::splitPoly(polygon* polyPtr, edge e)
{
	lateral branch = lateral::LEFT;
	unsigned int leftCount = 0, rightCount = 0;
	vect2 edgeStart, edgeEnd, intersectionPoint;
	if (polyPtr->leftChild == nullptr && polyPtr->rightChild == nullptr)		//If current polygon is childless
	{
		if (this->checkPolygonForSplitting(polyPtr->n, polyPtr->vertices, e))	//If polygon is split by current edge
		{
			polyPtr->leftChild				= new polygon;
			polyPtr->leftChild->n			= 0;
			polyPtr->leftChild->leftChild	= nullptr;
			polyPtr->leftChild->rightChild	= nullptr;
			//std::cout << "Polygon is split into leftChild..." << std::endl;

			polyPtr->rightChild				= new polygon;
			polyPtr->rightChild->n			= 0;
			polyPtr->rightChild->leftChild	= nullptr;
			polyPtr->rightChild->rightChild = nullptr;
			//std::cout << "...and rightChild." << std::endl;

			for (unsigned int i = 0; i < polyPtr->n; i++)
			{
				if (i < polyPtr->n - 1)
				{
					edgeStart	= polyPtr->vertices[i];
					edgeEnd		= polyPtr->vertices[i + 1];
				}
				else
				{
					edgeStart	= polyPtr->vertices[i];
					edgeEnd		= polyPtr->vertices[0];
				}
				if (iSect2dLine(edgeStart, edgeEnd, e, &intersectionPoint))
				{
					//std::cout << "Edge is split..." << std::endl;
					if (branch == lateral::LEFT)
					{
						polyPtr->leftChild->vertices[leftCount++]	= intersectionPoint;
						polyPtr->leftChild->n++;
						branch = lateral::RIGHT;
						polyPtr->rightChild->vertices[rightCount++] = intersectionPoint;
						polyPtr->rightChild->n++;
						polyPtr->rightChild->vertices[rightCount++] = edgeEnd;
						polyPtr->rightChild->n++;
					}
					else if (branch == lateral::RIGHT)
					{
						polyPtr->rightChild->vertices[rightCount++] = intersectionPoint;
						polyPtr->rightChild->n++;
						branch = lateral::LEFT;
						polyPtr->leftChild->vertices[leftCount++]	= intersectionPoint;
						polyPtr->leftChild->n++;
						polyPtr->leftChild->vertices[leftCount++]	= edgeEnd;
						polyPtr->leftChild->n++;
					}
				}
				else
				{
					//std::cout << "Edge doesn't need to be split..." << std::endl;
					if (branch == lateral::LEFT)
					{
						polyPtr->leftChild->vertices[leftCount++]	= edgeEnd;
						polyPtr->leftChild->n++;
					}
					else if (branch == lateral::RIGHT)
					{
						polyPtr->rightChild->vertices[rightCount++] = edgeEnd;
						polyPtr->rightChild->n++;
					}
				}
			}
		}
	}
	else if (polyPtr->leftChild != nullptr && polyPtr->rightChild != nullptr)
	{
		this->splitPoly(polyPtr->leftChild,		e);
		this->splitPoly(polyPtr->rightChild,	e);
	}
}


void Canvas::traversePolyTree(polygon* polyTree)
{
	if (polyTree->leftChild == nullptr && polyTree->rightChild == nullptr)
	{	
		//polyTree->colour = (polygonBuffer.size() * 4) << 16;
		polyTree->colour = (polygonBuffer.size() * 4);
		polygonBuffer.push_back(*polyTree);
	}
	else if (polyTree->leftChild != nullptr && polyTree->rightChild != nullptr)
	{
		traversePolyTree(polyTree->leftChild);
		traversePolyTree(polyTree->rightChild);
	}
}


void Canvas::buildPolyTree(polygon* rootPolyPtr, const std::vector<edge>& edges)
{
	std::cout << "Started building BSP tree..." << std::endl;
	std::cout << "Number of edges: " << edges.size() << std::endl;
	for (auto i = edges.begin(); i != edges.end(); ++i)
	{
		this->splitPoly(rootPolyPtr, *i);
	}
	this->traversePolyTree(rootPolyPtr);
}


void Canvas::drawPolyTree()
{
	for (auto i = polygonBuffer.begin(); i != polygonBuffer.end(); ++i)
	{
		auto nPoly = i->n;
		for (unsigned int p = 0; p < nPoly - 2; p++)
		{
			this->solidFillTriangle(i->vertices[0].onScreen(scale), i->vertices[p + 1].onScreen(scale), i->vertices[p + 2].onScreen(scale), i->colour);
		}
	}
}


void Canvas::exportPng(const std::string& fileName)
{
	std::vector<uint8_t> rawBuffer;

	uint8_t a, r, g, b;
	Uint32 colour;

	for (int i = 0; i < width * height; i++)
	{
		colour = pixelBuffer[i];

		a = colour >> 24	& 0xFF;
		r = colour >> 16	& 0xFF;
		g = colour >> 8		& 0xFF;
		b = colour			& 0xFF;

		rawBuffer.push_back(a);
		rawBuffer.push_back(r);
		rawBuffer.push_back(g);
		rawBuffer.push_back(b);
	}

	std::vector<std::uint8_t> imageBuffer;
	lodepng::encode(imageBuffer, rawBuffer, width, height);
	lodepng::save_file(imageBuffer, fileName);

	std::cout << "Image file saved..." << std::endl;
}
