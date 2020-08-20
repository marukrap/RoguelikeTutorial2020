#pragma once

#include "Menu.hpp"

#include <vector>
#include <string>
#include <functional>

class Game;

class PauseMenu : public Menu
{
public:
	explicit PauseMenu(Game& game);

	void handleKeys(SDL_Keycode key) override;
	void draw(Console& console) override;

private:
	void selectPrevious();
	void selectNext();

private:
	Game& m_game;
	std::vector<std::string> m_buttons;
	std::vector<std::function<void()>> m_callbacks;
	std::size_t m_selected = 0;
};
