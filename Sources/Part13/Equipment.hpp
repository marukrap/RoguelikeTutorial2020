#pragma once

#include <array>

class Item;
class Equippable;

class Equipment
{
public:
	enum Slot
	{
		None = -1,
		MainHand,
		OffHand,
		Body,
		NumSlots,
	};

public:
	Equipment();

	Equippable* get(int slot) const;
	int getEquippedSlot(const Item& item) const;

	void equip(Equippable* item);
	void unequip(int slot);

	int getMaxHpBonus() const;
	int getAttackBonus() const;
	int getDefenseBonus() const;

public:
	std::array<Equippable*, NumSlots> m_items;
};
