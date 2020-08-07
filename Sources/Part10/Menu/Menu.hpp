#pragma once

#include <SDL2/SDL.h>

class Console;

class Menu
{
public:
	virtual ~Menu() = default;

	virtual void handleKeys(SDL_Keycode key) = 0;
	virtual void draw(Console& console) = 0;
};
