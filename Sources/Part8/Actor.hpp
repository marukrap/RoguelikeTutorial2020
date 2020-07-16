#pragma once

#include "Entity.hpp"
#include "Inventory.hpp"

struct ActorData;

class Actor : public Entity
{
public:
	explicit Actor(std::string_view name);

	std::string_view getName() const override;

	bool isDestroyed() const;
	int getHp() const;
	int getMaxHp() const;

	void takeDamage(int damage);
	void restoreHp(int points);
	void attack(Actor& target);

	Actor* getTarget() const;
	void setTarget(Actor* target);
	void updateAi();

	Inventory* getInventory();

private:
	const std::string_view m_name;
	const ActorData& m_data;

	int m_hp;
	int m_maxHp;
	int m_attack;
	int m_defense;

	Actor* m_target = nullptr;
	std::unique_ptr<Inventory> m_inventory = nullptr;
};
