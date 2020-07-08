#include "Entity.hpp"
#include "World.hpp"
#include "Engine/Direction.hpp"

#include <unordered_map>
#include <algorithm> // max

struct EntityData
{
	char ch;
	Color color;
	int hp;
	int attack;
	int defense;
};

namespace
{
	const std::unordered_map<std::string_view, EntityData> Table =
	{
		{ "you",   { '@', 0xFFFFFF, 30, 5, 2 } },
		{ "orc",   { 'o', 0x14A02E, 10, 3, 0 } },
		{ "troll", { 'T', 0x1A7A3E, 16, 4, 1 } },
	};
}

World* Entity::s_world = nullptr;

Entity::Entity(std::string_view name)
	: m_name(name)
	, m_data(Table.at(name))
	, m_ch(m_data.ch)
	, m_color(m_data.color)
	, m_hp(m_data.hp)
	, m_maxHp(m_data.hp)
	, m_attack(m_data.attack)
	, m_defense(m_data.defense)
{
}

char Entity::getChar() const
{
	return m_ch;
}

Color Entity::getColor() const
{
	return m_color;
}

std::string_view Entity::getName() const
{
	return m_data.name;
}

std::string Entity::getTheName() const
{
	std::string theName;

	if (m_data.name != "you")
		theName += "the ";

	theName += m_data.name;

	return theName;
}

Vec2i Entity::getPosition() const
{
	return m_position;
}

void Entity::setPosition(int x, int y)
{
	m_position.x = x;
	m_position.y = y;
}

void Entity::setPosition(const Vec2i& position)
{
	setPosition(position.x, position.y);
}

void Entity::move(int dx, int dy)
{
	setPosition(m_position.x + dx, m_position.y + dy);
}

int Entity::getHp() const
{
	return m_hp;
}

int Entity::getMaxHp() const
{
	return m_maxHp;
}

bool Entity::isDestroyed() const
{
	return m_hp <= 0;
}

void Entity::takeDamage(int damage)
{
	if (damage > 0)
		m_hp -= damage;
}

void Entity::attack(Entity& target)
{
	const int damage = std::max(0, m_attack - target.m_defense);
	target.takeDamage(damage);

	std::string message = getTheName() + " hit " + target.getTheName()
		+ " for " + std::to_string(damage) + " damage.";

	s_world->addMessage(std::move(message));

	if (target.isDestroyed())
	{
		if (&target == s_world->getPlayerEntity())
			message = "you died.";
		else
			message = target.getTheName() + " is dead.";

		s_world->addMessage(std::move(message));
	}
}

Entity* Entity::getTarget() const
{
	return m_target;
}

void Entity::setTarget(Entity* target)
{
	m_target = target;
}

void Entity::updateAi()
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

			if (s_world->getEntity(nextPos))
			{
				const Direction nextDir = nextPos - m_position;
				const Vec2i leftPos = m_position + nextDir.left45();
				const Vec2i rightPos = m_position + nextDir.right45();

				if (s_world->isPassable(leftPos) && !s_world->getEntity(leftPos))
					setPosition(leftPos);
				else if (s_world->isPassable(rightPos) && !s_world->getEntity(rightPos))
					setPosition(rightPos);
			}

			else
				setPosition(nextPos);

			s_world->openDoor(m_position);
		}
	}
}

void Entity::setWorld(World& world)
{
	s_world = &world;
}
