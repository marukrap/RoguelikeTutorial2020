#include "Equippable.hpp"
#include "World.hpp"

#include <unordered_map>

struct EquippableData : public ItemData
{
	int slot;
	int maxHpBonus;
	int attackBonus;
	int defenseBonus;
};

namespace
{
	const std::unordered_map<std::string, EquippableData> Table =
	{
		{ "dagger",        { { ')', 0xFFD541, "A simple dagger.", &Item::equip     }, Equipment::MainHand, 0, 2, 0 } },
		{ "sword",         { { ')', 0xFFD541, "A basic sword.", &Item::equip       }, Equipment::MainHand, 0, 3, 0 } },
		{ "leather armor", { { '[', 0xFFD541, "Better than nothing.", &Item::equip }, Equipment::Body,     0, 0, 1 } },
	};
}

Equippable::Equippable(const std::string& name, const EquippableData& data)
	: Item(name, data)
	, m_equippableData(data)
{
}

int Equippable::getSlot() const
{
	return m_equippableData.slot;
}

int Equippable::getMaxHpBonus() const
{
	return m_equippableData.maxHpBonus;
}

int Equippable::getAttackBonus() const
{
	return m_equippableData.attackBonus;
}

int Equippable::getDefenseBonus() const
{
	return m_equippableData.defenseBonus;
}

void Equippable::equip(Actor& actor)
{
	Equipment* equipment = actor.getEquipment();
	Equippable* old = equipment->get(getSlot());

	if (old)
	{
		// Remove
		equipment->unequip(getSlot());
		s_world->addMessage("you removed " + old->getTheName() + ".");
	}

	if (old != this)
	{
		// Equip
		equipment->equip(this);
		s_world->addMessage("you equipped " + getTheName() + ".");
	}

	// HACK: clamp max hp
	if (actor.getHp() > actor.getMaxHp())
		actor.restoreHp(0);
}

const EquippableData* getEquippableData(const std::string& name)
{
	const auto found = Table.find(name);
	if (found != Table.end())
		return &found->second;

	return nullptr;
}
