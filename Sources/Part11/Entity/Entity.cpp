#include "Entity.hpp"

World* Entity::s_world = nullptr;

char Entity::getChar() const
{
	return m_ch;
}

Color Entity::getColor() const
{
	return m_color;
}

std::string Entity::getAName() const
{
	std::string aName(getName());

	// isVowel
	if (std::string_view("aeiou").find(aName[0]) != std::string_view::npos)
		aName = "an " + aName;
	else
		aName = "a " + aName;

	return aName;
}

std::string Entity::getTheName() const
{
	std::string theName(getName());

	if (theName != "you")
		theName = "the " + theName;

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

void Entity::save(std::ostream& os)
{
	serialize(os, m_position);
}

void Entity::load(std::istream& is)
{
	deserialize(is, m_position);
}

void Entity::setWorld(World& world)
{
	s_world = &world;
}
