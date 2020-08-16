#pragma once

#include "Menu.hpp"

#include <vector>
#include <string>

class World;

class LevelUpMenu : public Menu
{
public:
	explicit LevelUpMenu(World& world);

	void handleKeys(SDL_Keycode key) override;
	void draw(Console& console) override;

private:
	void selectPrevious();
	void selectNext();
	void activate();

private:
	World& m_world;
	std::vector<std::string> m_buttons;
	std::size_t m_selected = 0;
};
