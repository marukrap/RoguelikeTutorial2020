#include "Player.hpp"
#include "World.hpp"

Player::Player()
	: Actor("you")
	, m_inventory(26)
{
	m_maxHp = Actor::getMaxHp();
	m_attack = Actor::getAttack();
	m_defense = Actor::getDefense();
}

int Player::getLevel() const
{
	return m_level;
}

int Player::getXp() const
{
	return m_xp;
}

int Player::getXpToNextLevel() const
{
	constexpr int levelUpBase = 200;
	constexpr int levelUpFactor = 150;

	return levelUpBase + m_level * levelUpFactor;
}

int Player::getMaxHp() const
{
	return m_maxHp;
}

int Player::getAttack() const
{
	return m_attack;
}

int Player::getDefense() const
{
	return m_defense;
}

void Player::increaseMaxHp(int points)
{
	m_maxHp += points;
}

void Player::increaseAttack(int points)
{
	m_attack += points;
}

void Player::increaseDefense(int points)
{
	m_defense += points;
}

void Player::attack(Actor& target)
{
	Actor::attack(target);

	if (target.isDestroyed())
	{
		const int xp = target.getXp();
		const int xpToNextLevel = getXpToNextLevel();

		m_xp += xp;
		s_world->addMessage("you gain " + std::to_string(xp) + " experience points.");

		if (m_xp >= xpToNextLevel)
		{
			m_xp -= xpToNextLevel;
			++m_level;
			s_world->openLevelUpMenu();
		}
	}
}

Inventory* Player::getInventory()
{
	return &m_inventory;
}

void Player::save(std::ostream& os)
{
	Actor::save(os);

	m_inventory.save(os);

	serialize(os, m_level);
	serialize(os, m_xp);
	serialize(os, m_maxHp);
	serialize(os, m_attack);
	serialize(os, m_defense);
}

void Player::load(std::istream& is)
{
	Actor::load(is);

	m_inventory.load(is);

	deserialize(is, m_level);
	deserialize(is, m_xp);
	deserialize(is, m_maxHp);
	deserialize(is, m_attack);
	deserialize(is, m_defense);
}
