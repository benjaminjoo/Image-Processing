#pragma once

#include <memory>
#include <vector>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "escapi.h"
#include "Canvas.h"
#include "EventHandler.h"
#include "Utilities.h"
#include "Texture.h"


class WebCam
{
private:
	int	width								= 320;
	int height								= 200;
	int brightness							= 200;
	int motionTriggerSensitivity			= 100;
	Uint32 backgroundColour					= 0x00000000;
	double screenScale						= 1.0f;
	double screenOffsetX					= 0.0f;
	double screenOffsetY					= 0.0f;

	bool isOn								= true;

	int nCameras							= 0;
	SimpleCapParams capture;

	int* oldFrame							= nullptr;
	int* newFrame							= nullptr;

	std::shared_ptr<Canvas> Screen			= nullptr;
	std::shared_ptr<EventHandler> Controls	= nullptr;
	std::shared_ptr<Texture> Stains			= nullptr;

	std::vector<int3> viewCircles;

	void projectUnaltered();
	void projectPattern(bool* pattern, int w, int h, int pacingH, int pacingV);
	void projectPatternNoiseReduction(bool* pattern, int w, int h, int pacingH, int pacingV);
	void projectContrast();
	void projectMovement();
	void projectNoiseReduction();
	void projectCircular(std::vector<int3> circles);
	void projectMandelbrot(double scale, double offsetX, double offsetY);

	void clearFrame(int* frame, int w, int h, int c);
	void copyFrame(int* frameA, int* frameB, int w, int h);

public:
	WebCam(int w, int h, Uint32 c);
	~WebCam();

	bool isRunning();
	void update();
	void addViewCircle(int3);
};

