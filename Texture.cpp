#include "Texture.h"
#include "Utilities.h"


Texture::Texture(const std::string& fileName)
{
	pixelData	= nullptr;
	width		= 0;
	height		= 0;
	SDL_Surface* textureData = IMG_Load(fileName.c_str());
	if (textureData == nullptr)
		std::cout << "Failed to load image file " << fileName.c_str() << std::endl;
	else
	{
		SDL_Surface* tempImage = SDL_ConvertSurfaceFormat(textureData, SDL_PIXELFORMAT_ARGB8888, 0);
		width		= textureData->w;
		height		= textureData->h;
		pixelData	= (Uint32*)tempImage->pixels;
	}
}


Texture::~Texture()
{
}


int Texture::getWidth()
{
	return width;
}


int Texture::getHeight()
{
	return height;
}


Uint32 Texture::getPixel(const double& u, const double& v)
{
	int x = (int)((double)width * (u - (int)u));
	int y = (int)((double)height * (v - (int)v));

	if (x >= width)
		x = width - 1;
	if (y >= height)
		y = height - 1;

	return pixelData[y * width + x];
}
