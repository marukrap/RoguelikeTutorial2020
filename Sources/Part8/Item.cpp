#include "Item.hpp"
#include "World.hpp"

#include <functional>
#include <unordered_map>

struct ItemData
{
	char ch;
	Color color;
	std::string_view description;
	std::function<void(Item&, Actor&)> apply;
};

namespace
{
	const std::unordered_map<std::string_view, ItemData> Table =
	{
		{ "potion of healing", { '!', 0xFFD541, "Restores HP", &Item::heal } },
	};
}

Item::Item(std::string_view name)
	: m_name(name)
	, m_data(Table.at(name))
{
	m_ch = m_data.ch;
	m_color = m_data.color;
}

std::string_view Item::getName() const
{
	return m_name;
}

std::string Item::getAName() const
{
	if (m_data.ch == '[') // Armor
		return std::string(m_name);

	return Entity::getAName();
}

std::string_view Item::getDescription() const
{
	return m_data.description;
}

int Item::getCount() const
{
	return m_count;
}

void Item::setCount(int count)
{
	m_count = count;
}

void Item::apply(Actor& actor)
{
	m_data.apply(*this, actor);
}

void Item::heal(Actor& actor)
{
	actor.restoreHp(actor.getMaxHp() / 2);
	s_world->addMessage(actor.getTheName() + " feel better.");
	--m_count;
}
