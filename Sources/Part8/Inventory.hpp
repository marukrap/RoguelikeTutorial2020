#pragma once

#include "Item.hpp"

#include <vector>

class World;
class Console;

class Inventory
{
public:
	explicit Inventory(std::size_t maxSize);

	std::size_t getMaxSize() const;
	std::size_t getNumItems() const;
	Item* at(std::size_t i);

	bool isEmpty() const;
	bool isFull() const;

	void pack(Item::Ptr item);
	Item::Ptr unpack(Item& item);
	Item::Ptr unpack(std::size_t i);

	int getGold() const;

private:
	int m_gold = 0;
	std::size_t m_maxSize;
	std::vector<Item::Ptr> m_items;
};

class InventoryMenu
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
	explicit InventoryMenu(Inventory& inventory);

	bool hasSelection() const;
	void select(std::size_t i);
	void selectPrevious();
	void selectNext();

	bool hasButtonSelection() const;
	void selectButton(std::size_t i);
	void selectPreviousButton();
	void selectNextButton();
	void activateButton(World& world);

	void draw(Console& console);

private:
	Inventory& m_inventory;
	int m_selectedItem = -1;
	int m_selectedButton = -1;
};
