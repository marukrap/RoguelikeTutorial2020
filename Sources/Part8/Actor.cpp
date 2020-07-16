#include "Actor.hpp"
#include "World.hpp"
#include "Engine/Direction.hpp"

#include <unordered_map>
#include <algorithm> // max

struct ActorData
{
	char ch;
	Color color;
	int hp;
	int attack;
	int defense;
};

namespace
{
	const std::unordered_map<std::string_view, ActorData> Table =
	{
		{ "you",   { '@', 0xFFFFFF, 30, 5, 2 } },
		{ "orc",   { 'o', 0x14A02E, 10, 3, 0 } },
		{ "troll", { 'T', 0x1A7A3E, 16, 4, 1 } },
	};
}

Actor::Actor(std::string_view name)
	: m_name(name)
	, m_data(Table.at(name))
	, m_hp(m_data.hp)
	, m_maxHp(m_data.hp)
	, m_attack(m_data.attack)
	, m_defense(m_data.defense)
{
	m_ch = m_data.ch;
	m_color = m_data.color;

	if (m_name == "you")
		m_inventory = std::make_unique<Inventory>(26);
}

std::string_view Actor::getName() const
{
	return m_name;
}

bool Actor::isDestroyed() const
{
	return m_hp <= 0;
}

int Actor::getHp() const
{
	return m_hp;
}

int Actor::getMaxHp() const
{
	return m_maxHp;
}

void Actor::takeDamage(int damage)
{
	if (damage > 0)
		m_hp -= damage;
}

void Actor::restoreHp(int points)
{
	m_hp = std::min(m_hp + points, m_maxHp);
}

void Actor::attack(Actor& target)
{
	const int damage = std::max(0, m_attack - target.m_defense);
	target.takeDamage(damage);

	std::string message = getTheName() + " hit " + target.getTheName()
		+ " for " + std::to_string(damage) + " damage.";

	s_world->addMessage(std::move(message));

	if (target.isDestroyed())
	{
		if (&target == s_world->getPlayerActor())
			message = "you died.";
		else
			message = target.getTheName() + " is dead.";

		s_world->addMessage(std::move(message));
		s_world->closeDoor(target.getPosition());
	}
}

Actor* Actor::getTarget() const
{
	return m_target;
}

void Actor::setTarget(Actor* target)
{
	m_target = target;
}

void Actor::updateAi()
{
	if (!m_target)
		return;

	const Vec2i targetPos = m_target->getPosition();

	if ((targetPos - m_position).lengthSquared() <= 2)
		attack(*m_target);

	else
	{
		const auto path = s_world->findPath(targetPos, m_position);

		if (path.size() > 2)
		{
			s_world->closeDoor(m_position);

			const Vec2i nextPos = path[1];

			if (s_world->getActor(nextPos))
			{
				const Direction nextDir = nextPos - m_position;
				const Vec2i leftPos = m_position + nextDir.left45();
				const Vec2i rightPos = m_position + nextDir.right45();

				if (s_world->isPassable(leftPos) && !s_world->getActor(leftPos))
					setPosition(leftPos);
				else if (s_world->isPassable(rightPos) && !s_world->getActor(rightPos))
					setPosition(rightPos);
			}

			else
				setPosition(nextPos);

			s_world->openDoor(m_position);
		}
	}
}

Inventory* Actor::getInventory()
{
	return m_inventory.get();
}
