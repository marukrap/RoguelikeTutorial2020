#include "Item.hpp"
#include "Equippable.hpp"
#include "World.hpp"
#include "Engine/Rng.hpp"

#include <unordered_map>

namespace
{
	const std::unordered_map<std::string, ItemData> Table =
	{
		{ "potion of healing",   { '!', 0xFFD541, "Restores some HP.", &Item::heal } },
		{ "potion of confusion", { '!', 0xFFD541, "Makes you confused.", &Item::confuse } },
	};
}

Item::Item(const std::string& name, const ItemData& data)
	: m_name(name)
	, m_data(data)
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

	if (actor.isPlayer())
		s_world->addMessage("you feel better.");
	else
		s_world->addMessage(actor.getTheName() + " looks better.");

	--m_count;
}

void Item::confuse(Actor& actor)
{
	actor.addStatusEffect(StatusEffect::Confused, randomInt(5, 10));

	if (actor.isPlayer())
		s_world->addMessage("you're confused.");
	else
		s_world->addMessage(actor.getTheName() + " looks confused.");

	--m_count;
}

void Item::equip(Actor& actor)
{
}

void Item::throwAt(Actor& actor)
{
	if (getChar() == '!') // getType() == Potion
	{
		m_data.apply(*this, actor);
		--m_count;
	}
}

void Item::save(std::ostream& os)
{
	serialize(os, m_name);
	serialize(os, m_count);

	Entity::save(os);
}

void Item::load(std::istream& is)
{
	//deserialize(is, m_name);
	deserialize(is, m_count);

	Entity::load(is);
}

Item::Ptr Item::createItem(const std::string& name)
{
	const auto found = Table.find(name);
	if (found != Table.end())
		return std::make_unique<Item>(name, found->second);

	if (const EquippableData* data = getEquippableData(name))
		return std::make_unique<Equippable>(name, *data);

	return nullptr;
}
