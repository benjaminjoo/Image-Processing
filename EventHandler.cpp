#include <iostream>

#include "EventHandler.h"


EventHandler::EventHandler()
{
	std::cout << "EventHandler created sussecfully..." << std::endl;
}


EventHandler::EventHandler(const double& turn_, const double& move_, const double& strf_, const double& sens_):
	turn(turn_), move(move_), strafe(strf_), sensitivity(sens_)
{
	std::cout << "EventHandler created sussecfully..." << std::endl;
}


EventHandler::~EventHandler()
{
	std::cout << "EventHandler destroyed sussecfully..." << std::endl;
}


bool EventHandler::isRunning()
{
	return running;
}


void EventHandler::HandleUserEvents()
{
	const Uint8* state = SDL_GetKeyboardState(nullptr);

	if (state[SDL_SCANCODE_UP] && !state[SDL_SCANCODE_DOWN])
		move = -step_speed;

	if (state[SDL_SCANCODE_DOWN] && !state[SDL_SCANCODE_UP])
		move = step_speed;

	if (state[SDL_SCANCODE_LEFT] && !state[SDL_SCANCODE_RIGHT])
		strafe = -step_speed;

	if (state[SDL_SCANCODE_RIGHT] && !state[SDL_SCANCODE_LEFT])
		strafe = step_speed;	

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				running = false;
				break;
			case SDLK_TAB:
				showStats = showStats ? false : true;
				break;
			case SDLK_p:
				paused = paused ? false : true;
				break;
			case SDLK_a:
				styleFlag = 0;
				break;
			case SDLK_b:
				styleFlag = 1;
				break;
			case SDLK_c:
				styleFlag = 2;
				break;
			case SDLK_h:
				styleFlag = 3;
				break;
			case SDLK_m:
				styleFlag = 4;
				break;
			case SDLK_s:
				styleFlag = 5;
				break;
			case SDLK_f:
				styleFlag = 6;
				break;
			case SDLK_KP_PLUS:
				{
				if (brightness < 750)
					brightness += 10;
				}
				break;
			case SDLK_KP_MINUS:
				{
				if (brightness > 10)
					brightness -= 10;
				}
				break;
			case SDLK_PAGEUP:
				{
					if (mtSens < 500000)
						mtSens += 5;
				}
				break;
			case SDLK_PAGEDOWN:
				{
					if (mtSens > 5)
						mtSens -= 5;
				}
				break;
			case SDLK_KP_MULTIPLY:
				sensitivity *= 1.1f;
				break;
			case SDLK_KP_DIVIDE:
				sensitivity *= 0.9f;
				break;
			default:
				break;
			}
		}
	}


	if (event.type == SDL_MOUSEMOTION)
	{
		turn = (double)event.motion.x * sensitivity;
	}

	if (event.button.button == SDL_BUTTON_LEFT && event.button.state == SDL_PRESSED)
	{
		firing = true;
	}

	if (event.button.button == SDL_BUTTON_LEFT && event.button.state == SDL_RELEASED)
	{
		firing = false;
	}
}


void EventHandler::ceaseMotion()
{
	move	= 0.0f;
	strafe	= 0.0f;	
}
