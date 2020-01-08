#pragma once

#include <vector>
#include <string>
#include <memory>
#include <SDL/SDL.h>
#include "Math.h"
#include "Utilities.h"
#include "Texture.h"

class Canvas
{
private:

	int						width;
	int						height;

	double					scale;

	int						fontSize = 8;

	SDL_Window*				window;
	SDL_Renderer*			screen;
	SDL_Texture*			texture;

	bool					closed = false;

	std::vector<polygon>	polygonBuffer;

public:

	Uint32*			pixelBuffer = nullptr;
	Uint32			clearColour = 0;

public:

	Canvas(const int& w, const int& h, const double& s, const std::string& title);
	~Canvas();

	void setClearColour(Uint32 col);

	void clear();
	void update();
	void putPixel(int x, int y, Uint32 colour);
	void putPixel(int n, Uint32 colour);

	int getWidth();
	int getHeight();
	double getScale();

	void drawLine(const screenCoord& startP, const screenCoord& endP, const Uint32& colour);
	void drawBoundingBox(boundingBoxScreen BB, const Uint32& colour);
	void drawCircle(const screenCoord& centreP, const int& radius, const Uint32& colour);
	void drawRectangle(const screenCoord& topLeft, const screenCoord& bottomRight, const Uint32& colour);
	void solidFillTriangle(const screenCoord& a, const screenCoord& b, const screenCoord& c, const Uint32& colour);
	void solidFillConvexPoly(unsigned int n, const screenCoord* P, const Uint32& colour);
	void renderTriangle(const triangle2& t, vect2 A, vect2 B, vect2 C, const double& scale, Texture* texture);

	bool* GetSingleDigit_8(char letter_No);
	void displayFloat(double value, int dec, int shiftH, int shiftV, Uint32 colour);
	void displayInt(int value, int font_size, int shiftH, int shiftV, lateral alignment, Uint32 colour);
	void displayString(const char* text, int shiftH, int shiftV, lateral alignment, Uint32 colour);


	bool checkPolygonForSplitting(int n, vect2* V, edge e);
	bool iSect2dLine(vect2 a, vect2 b, edge e, vect2* r);
	void splitPoly(polygon* polyPtr, edge e);
	void buildPolyTree(polygon* rootPolyPtr, const std::vector<edge>& edges);
	void traversePolyTree(polygon* polyTree);
	void drawPolyTree();

	void exportPng(const std::string& fileName);
};																							

