#pragma once

#include <SDL/SDL.h>

class EventHandler
{
public:

	SDL_Event event		= { 0 };

	int styleFlag		= 0;
	int brightness		= 200;
	int mtSens			= 100;

	bool running		= true;
	bool paused			= false;
	bool firing			= false;
	bool showStats		= false;

	double turn			= 0.0f;
	double move			= 0.0f;
	double strafe		= 0.0f;

	double step_speed	= 0.1f;
	double turn_speed	= 2.0f;

	double sensitivity	= 1.0f;

	EventHandler();
	EventHandler(const double& turn_, const double& move_, const double& strf_, const double& sens_);
	~EventHandler();

	bool isRunning();

	void HandleUserEvents();
	void ceaseMotion();
};

