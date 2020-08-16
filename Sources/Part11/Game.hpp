#pragma once

#include "Engine/Renderer.hpp"
#include "World.hpp"

class Console;

class Game
{
public:
	Game(SDL_Window& window, Console& console);
	~Game();

	void save(std::ostream& os);

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

	World m_world;
};
