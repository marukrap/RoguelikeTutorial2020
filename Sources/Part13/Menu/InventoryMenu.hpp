#pragma once

#include "Menu.hpp"

#include <cstddef>

class World;
class Inventory;
class Console;

class InventoryMenu : public Menu
{
public:
	enum ButtonType
	{
		Use,
		Drop,
		Throw,
		NumButtons,
	};

public:
	InventoryMenu(World& world, Inventory& inventory);

	void handleKeys(SDL_Keycode key) override;
	void draw(Console& console) override;

	void selectButton(std::size_t i);

private:
	bool hasSelection() const;
	void select(std::size_t i);
	void selectPrevious();
	void selectNext();

	bool hasButtonSelection() const;
	void selectPreviousButton();
	void selectNextButton();
	void activateButton();

private:
	World& m_world;
	Inventory& m_inventory;
	int m_selectedItem = -1;
	int m_selectedButton = -1;
};
