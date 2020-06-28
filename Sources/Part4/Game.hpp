#pragma once

#include "Entity.hpp"
#include "Map.hpp"
#include "Engine/Renderer.hpp"
#include "Engine/Fov.hpp"

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

	static constexpr int m_fovRange = 10;

	std::unique_ptr<Map> m_map = nullptr;
	std::unique_ptr<Fov> m_fov = nullptr;
	std::vector<std::unique_ptr<Entity>> m_entities;
	Entity* m_player = nullptr;
	bool m_needsFovUpdate = true;
};
