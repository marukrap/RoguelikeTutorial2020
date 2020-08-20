#pragma once

#include "Engine/Renderer.hpp"
#include "World.hpp"

class Console;

class Game
{
public:
	Game(SDL_Window& window, Console& console);

	bool isRunning() const;
	void tick();

	World* getWorld();
	void createWorld();
	void loadSavefile();

	void closeMenu();
	void openMenu(std::unique_ptr<Menu> menu);
	void openMainMenu();
	void openPauseMenu();
	void openInventory(SDL_Keycode key);

	bool isSaving();
	void save(bool quit);

private:
	void processInput();
	void update();
	void render();

private:
	SDL_Window& m_window;
	Console& m_console;
	Renderer m_renderer;
	bool m_running = true;

	std::unique_ptr<World> m_world = nullptr;
	std::unique_ptr<Menu> m_menu = nullptr;
};
