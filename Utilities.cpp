#include "Utilities.h"


bool isEven(int n)
{
	return !(n % 2);
}


Uint32 argbColour(unsigned char a, unsigned char r, unsigned char g, unsigned char b)
{
	return ((a << 24) | (r << 16) | (g << 8) | (b << 0));
}


double getGrayScale(colour32 a)
{
	return ((double)a.c[2] / 255.0f + (double)a.c[1] / 255.0f + (double)a.c[0] / 255.0f) / 3;
}


colour32 getGrayScalePixel(double s)
{
	colour32 result;

	result.c[3] = 0;
	result.c[2] = result.c[1] = result.c[0] = (unsigned char)((double)255 * s);

	return result;
}


colour32 getGrayScalePixelDiscreet(double s, char d)
{
	colour32 result;

	unsigned char colourValue = static_cast<unsigned char>((255.0f * s) / d) * d;

	result.c[3] = 0;
	result.c[2] = result.c[1] = result.c[0] = colourValue;

	return result;
}


colour32 getGrayScalePixelDiscreet(colour32 c, char d)
{
	double s = getGrayScale(c);

	colour32 result;

	unsigned char colourValue = static_cast<unsigned char>((255.0f * s) / d) * d;

	result.c[3] = 0;
	result.c[2] = 0;
	result.c[1] = 0;
	result.c[0] = colourValue;

	return result;
}


double colourLength(colour32 c)
{
	return sqrt(c.c[2] * c.c[2] + c.c[1] * c.c[1] + c.c[0] * c.c[0]);
}


double colourDifference(colour32 a, colour32 b)
{
	unsigned dot = a.c[2] * b.c[2] + a.c[1] * b.c[1] + a.c[0] * b.c[0];
	return (double)dot / (colourLength(a) * colourLength(b));
}


colour32 subtractColour(colour32 a, colour32 b)
{
	colour32 result;

	result.c[3] = 0;
	result.c[2] = abs(a.c[2] - b.c[2]);
	result.c[1] = abs(a.c[1] - b.c[1]);
	result.c[0] = abs(a.c[0] - b.c[0]);

	clampColour(result);

	return result;
}


colour32 addColour(colour32 a, colour32 b)
{
	colour32 result;

	result.c[3] = 0;
	result.c[2] = a.c[2] + b.c[2];
	result.c[1] = a.c[1] + b.c[1];
	result.c[0] = a.c[0] + b.c[0];

	clampColour(result);

	return result;
}


colour32 scaleColour(colour32 a, double s)
{
	colour32 result;

	result.c[3] = 0;
	result.c[2] = (unsigned char)(a.c[2] * s);
	result.c[1] = (unsigned char)(a.c[1] * s);
	result.c[0] = (unsigned char)(a.c[0] * s);

	clampColour(result);

	return result;
}


void clampColour(colour32& col)
{
	for (int i = 0; i < 3; i++)
	{
		if (col.c[i] > 255)
			col.c[i] = 255;
		if (col.c[i] < 0)
			col.c[i] = 0;
	}
}


void dominantColour(colour32& a)
{
	int dominant = 0;

	for (int i = 1; i < 3; i++)
		if (a.c[i] > a.c[dominant])
			dominant = i;

	for (int i = 0; i < 3; i++)
		if (i == dominant)
			a.c[i] = 255;
		else
			a.c[i] = 63;

}


void alterColour(colour32& a, char d)
{
	switch (d)
	{
		case 'r':
		{
			a.c[1] = 0;
			a.c[0] = 0;
		}
		break;
		case 'g':
		{
			a.c[2] = 0;
			a.c[0] = 0;
		}
		break;
		case 'b':
		{
			a.c[2] = 0;
			a.c[1] = 0;
		}
		break;
	}
}


void convertToGrayScale(colour32& a)
{
	double s = (static_cast<double>(a.c[2]) * 0.3f + static_cast<double>(a.c[1]) * 0.59f + static_cast<double>(a.c[0]) * 0.11f) / (3 * 255.0f) * 3.0f;
	if (s > 1.0f)
		s = 1.0f;
	a.c[2] = a.c[1] = a.c[0] = static_cast<unsigned char>(static_cast<double>(255) * s);
}


int sign(const double& a)
{
	int t;
	if (a > 0) { t = 1; }
	else if (a == 0) { t = 0; }
	else { t = -1; }

	return t;
}


double distPoint2Line(vect2 P, edge L)
{
	return abs((L.startP - P) * L.normal);
}


double distPoint2Point(vect2 A, vect2 B)
{
	return sqrt((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y));
}


bool pointIsAroundLine(vect2 P, edge L)
{
	double sA = (L.endP - L.startP) * (L.startP - P);
	double sB = (L.endP - L.startP) * (L.endP - P);

	return (sign(sA) != sign(sB)) ? true : false;
}


int GetYMax3(screenCoord* p)
{
	int yMax = p[0].y;
	for (int i = 1; i < 3; i++)
	{
		if (p[i].y > yMax) { yMax = p[i].y; }
	}
	return yMax;
}


int GetYMin3(screenCoord* p)
{
	int yMin = p[0].y;
	for (int i = 1; i < 3; i++)
	{
		if (p[i].y < yMin) { yMin = p[i].y; }
	}
	return yMin;
}


int power(int number, unsigned int p)
{
	int result = 1;
	for (unsigned int i = 0; i < p; i++)
	{
		result *= number;
	}
	return result;
}


double pow10(int n)
{
	double result = 1.0;
	if (n > 0)
		for (int i = 0; i < n; i++)
			result *= 10;
	else if (n < 0)
		for (int i = 0; i > n; i--)
			result /= 10;
	return result;
}


std::shared_ptr<int[]> getFractionals(double number, int nDecimals)
{
	int dec;
	if (number > 0)
	{
		dec = int((number - (int)(number)) * pow10(nDecimals));
	}
	else dec = 0;
	std::shared_ptr<int[]> decimalDigits(new int[nDecimals]);
	for (int i = 0; i < nDecimals; i++)
	{
		if (i < nDecimals)
		{
			decimalDigits[i] = (int)(dec / pow10(nDecimals - 1 - i));
			int rem = dec % (int)(pow10(nDecimals - 1 - i));
			dec = rem;
		}
	}
	return decimalDigits;
}


std::shared_ptr<int[]> getIntegers(double number, int* n)
{
	int rawInt = (int)(number);
	int nDigits = 0;
	while (rawInt > 0)
	{
		rawInt /= 10;
		nDigits++;
	}
	std::shared_ptr<int[]> intDigits(new int[nDigits]);
	rawInt = (int)(number);
	for (int i = nDigits - 1; i >= 0; i--)
	{
		intDigits[i] = rawInt % 10;
		rawInt /= 10;
	}
	*n = nDigits;
	return intDigits;
}