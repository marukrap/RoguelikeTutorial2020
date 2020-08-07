#pragma once

#include "Entity.hpp"
#include "Inventory.hpp"

struct ActorData;

enum class StatusEffect
{
	Confused,
};

class Actor : public Entity
{
public:
	explicit Actor(const std::string& name);

	std::string_view getName() const override;

	bool isPlayer() const;
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

	bool hasStatusEffect(StatusEffect effect) const;
	void addStatusEffect(StatusEffect effect, int duration);
	void removeStatusEffect(StatusEffect effect);
	void clearStatusEffects();
	void finishTurn();

	void save(std::ostream& os) override;
	void load(std::istream& is) override;

private:
	const std::string m_name;
	const ActorData& m_data;

	int m_hp;
	int m_maxHp;
	int m_attack;
	int m_defense;

	Actor* m_target = nullptr;
	std::unique_ptr<Inventory> m_inventory = nullptr;
	std::vector<std::pair<StatusEffect, int>> m_statusEffects;
};
