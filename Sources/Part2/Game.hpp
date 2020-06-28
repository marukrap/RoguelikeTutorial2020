#pragma once

#include "Engine/Renderer.hpp"
#include "Entity.hpp"
#include "Map.hpp"

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

	std::unique_ptr<Map> m_map = nullptr;
	std::vector<std::unique_ptr<Entity>> m_entities;
	Entity* m_player = nullptr;
};
