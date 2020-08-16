#pragma once

#include "Menu.hpp"

#include <vector>
#include <string>
#include <functional>

class World;

class MainMenu : public Menu
{
public:
	explicit MainMenu(World& world);
	void setHasSavefile();

	void handleKeys(SDL_Keycode key) override;
	void draw(Console& console) override;

private:
	void selectPrevious();
	void selectNext();

private:
	World& m_world;
	std::vector<std::string> m_buttons;
	std::vector<std::function<void()>> m_callbacks;
	std::size_t m_selected = 0;
	bool m_hasSavefile = false;
};
