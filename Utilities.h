#pragma once

#include <SDL/SDL.h>
#include <memory>
#include "Math.h"


bool isEven(int n);


Uint32 argbColour(unsigned char a, unsigned char r, unsigned char g, unsigned char b);


struct int3
{
	int a = 0;
	int b = 0;
	int c = 0;
};


union colour32
{
	Uint32 argb = 0;
	unsigned char c[4];
};


double getGrayScale(colour32 a);


colour32 getGrayScalePixel(double s);


colour32 getGrayScalePixelDiscreet(double s, char d);


colour32 getGrayScalePixelDiscreet(colour32 c, char d);


double colourLength(colour32 c);


double colourDifference(colour32 a, colour32 b);


colour32 subtractColour(colour32 a, colour32 b);


colour32 addColour(colour32 a, colour32 b);


colour32 scaleColour(colour32 a, double s);


void clampColour(colour32& a);


void dominantColour(colour32& a);


void alterColour(colour32& a, char d);


void convertToGrayScale(colour32& a);


int sign(const double& a);


double distPoint2Line(vect2 P, edge L);


double distPoint2Point(vect2 A, vect2 B);


bool pointIsAroundLine(vect2 P, edge L);


int GetYMax3(screenCoord* p);


int GetYMin3(screenCoord* p);


int power(int number, unsigned int p);


double pow10(int n);


std::shared_ptr<int[]> getFractionals(double number, int nDecimals);


std::shared_ptr<int[]> getIntegers(double number, int* n);
