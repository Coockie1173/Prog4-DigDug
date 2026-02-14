#include <SDL3/SDL.h>
#include "InputManager.h"
#include "Debugger.h"

bool dae::InputManager::ProcessInput()
{
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_EVENT_QUIT) 
		{
			return false;
		}
		if (e.type == SDL_EVENT_KEY_DOWN) 
		{
			switch (e.key.key)
			{
			case SDLK_ESCAPE:
			{
				return false;
			}
			case SDLK_F6:
			{
				//clear all debug log info
				Debugger::GetInstance().DeleteAllLogs();
			}
			}
		}
		if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) 
		{
			
		}
		// etc...
	}

	return true;
}
