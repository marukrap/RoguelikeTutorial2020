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
	virtual int getMaxHp() const;
	virtual int getAttack() const;
	virtual int getDefense() const;
	int getXp() const;

	void takeDamage(int damage);
	void restoreHp(int points);
	virtual void attack(Actor& target);

	Actor* getTarget() const;
	void setTarget(Actor* target);
	void updateAi();

	virtual Inventory* getInventory();

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

	Actor* m_target = nullptr;
	std::vector<std::pair<StatusEffect, int>> m_statusEffects;
};
