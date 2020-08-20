#include "Equipment.hpp"
#include "Entity/Equippable.hpp"

Equipment::Equipment()
{
	m_items.fill(nullptr);
}

Equippable* Equipment::get(int slot) const
{
	return m_items[slot];
}

int Equipment::getEquippedSlot(const Item& item) const
{
	for (const auto& i : m_items)
	{
		if (i == &item)
			return i->getSlot();
	}

	return Slot::None;
}

void Equipment::equip(Equippable* item)
{
	m_items[item->getSlot()] = item;
}

void Equipment::unequip(int slot)
{
	m_items[slot] = nullptr;
}

int Equipment::getMaxHpBonus() const
{
	int bonus = 0;

	for (const auto& item : m_items)
	{
		if (item)
			bonus += item->getMaxHpBonus();
	}

	return bonus;
}

int Equipment::getAttackBonus() const
{
	int bonus = 0;

	for (const auto& item : m_items)
	{
		if (item)
			bonus += item->getAttackBonus();
	}

	return bonus;
}

int Equipment::getDefenseBonus() const
{
	int bonus = 0;

	for (const auto& item : m_items)
	{
		if (item)
			bonus += item->getDefenseBonus();
	}

	return bonus;
}
