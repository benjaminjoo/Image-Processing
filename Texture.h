#pragma once

#include <iostream>
#include <string>
#include <memory>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

class Texture
{
private:

	Uint32* pixelData;

	int width;
	int height;

public:
	Texture(const std::string& fileName);
	~Texture();

	int getWidth();
	int getHeight();

	Uint32 getPixel(const double& u, const double& v);
};

