#pragma once

#include "Entity/Item.hpp"

#include <vector>

class Inventory : public Serializable
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

	void save(std::ostream& os) override;
	void load(std::istream& is) override;

private:
	int m_gold = 0;
	std::size_t m_maxSize;
	std::vector<Item::Ptr> m_items;
};
