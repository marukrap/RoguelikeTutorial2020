#pragma once

#include "Engine/Renderer.hpp"

class Console;

class Game
{
public:
	Game(SDL_Window& window, Console& console);

	bool isRunning() const;
	void tick();

private:
	void processInput();
	void update();
	void render();

private:
	SDL_Window& m_window;
	Console& m_console;
	Renderer m_renderer;
	bool m_running = true;

	int m_playerX = 0;
	int m_playerY = 0;
};
