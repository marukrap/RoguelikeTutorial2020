#include "Entity.hpp"

Entity::Entity(char ch, Color color)
	: m_ch(ch)
	, m_color(color)
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

Vec2i Entity::getPosition() const
{
	return m_position;
}

void Entity::setPosition(int x, int y)
{
	m_position.x = x;
	m_position.y = y;
}

void Entity::move(int dx, int dy)
{
	m_position.x += dx;
	m_position.y += dy;
}
