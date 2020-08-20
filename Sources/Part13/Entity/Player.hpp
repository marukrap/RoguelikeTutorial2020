#pragma once

#include "Actor.hpp"

class Player : public Actor
{
public:
	Player();

	int getLevel() const;
	int getXp() const;
	int getXpToNextLevel() const;

	int getMaxHp() const override;
	int getAttack() const override;
	int getDefense() const override;

	void increaseMaxHp(int points);
	void increaseAttack(int points);
	void increaseDefense(int points);

	void attack(Actor& target) override;
	Inventory* getInventory() override;
	Equipment* getEquipment() override;

	void save(std::ostream& os) override;
	void load(std::istream& is) override;

private:
	Inventory m_inventory;
	Equipment m_equipment;
	int m_level = 1;
	int m_xp = 0;
	int m_maxHp = 0;
	int m_attack = 0;
	int m_defense = 0;
};
