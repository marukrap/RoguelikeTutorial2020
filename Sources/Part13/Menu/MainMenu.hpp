#pragma once

#include "Menu.hpp"

#include <vector>
#include <string>
#include <functional>

class Game;

class MainMenu : public Menu
{
public:
	MainMenu(Game& game, bool hasSavefile);

	void handleKeys(SDL_Keycode key) override;
	void draw(Console& console) override;

private:
	void selectPrevious();
	void selectNext();

private:
	Game& m_game;
	std::vector<std::string> m_buttons;
	std::vector<std::function<void()>> m_callbacks;
	bool m_hasSavefile = false;
	std::size_t m_selected = 0;
};
