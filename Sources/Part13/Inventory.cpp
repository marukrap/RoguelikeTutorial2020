#include "Inventory.hpp"

#include <algorithm> // find_if

Inventory::Inventory(std::size_t maxSize)
	: m_maxSize(maxSize)
{
}

std::size_t Inventory::getMaxSize() const
{
	return m_maxSize;
}

std::size_t Inventory::getNumItems() const
{
	return m_items.size();
}

Item* Inventory::at(std::size_t i)
{
	if (i < m_items.size())
		return m_items[i].get();

	return nullptr;
}

bool Inventory::isEmpty() const
{
	return m_items.empty();
}

bool Inventory::isFull() const
{
	return m_items.size() >= m_maxSize;
}

void Inventory::pack(Item::Ptr item)
{
	m_items.push_back(std::move(item));
}

Item::Ptr Inventory::unpack(Item& item)
{
	const auto found = std::find_if(m_items.begin(), m_items.end(),
		[&] (const auto& i) { return i.get() == &item; });

	auto result = std::move(*found);
	m_items.erase(found);

	return result;
}

Item::Ptr Inventory::unpack(std::size_t i)
{
	auto result = std::move(m_items[i]);
	m_items.erase(m_items.begin() + i);

	return result;
}

int Inventory::getGold() const
{
	return m_gold;
}

void Inventory::save(std::ostream& os)
{
	serialize(os, m_gold);

	const std::size_t numItems = m_items.size();
	serialize(os, numItems);

	for (std::size_t i = 0; i < numItems; ++i)
		m_items[i]->save(os);
}

void Inventory::load(std::istream& is)
{
	deserialize(is, m_gold);

	std::size_t numItems;
	deserialize(is, numItems);

	m_items.resize(numItems);
	for (std::size_t i = 0; i < numItems; ++i)
	{
		std::string name;
		deserialize(is, name);
		m_items[i] = Item::createItem(name);
		m_items[i]->load(is);
	}
}
