#pragma once

#include "Item.hpp"

struct EquippableData;

class Equippable : public Item
{
public:
	Equippable(const std::string& name, const EquippableData& data);

	int getSlot() const;
	int getMaxHpBonus() const;
	int getAttackBonus() const;
	int getDefenseBonus() const;

	void equip(Actor& actor) override;

private:
	const EquippableData& m_equippableData;
};

const EquippableData* getEquippableData(const std::string& name);
