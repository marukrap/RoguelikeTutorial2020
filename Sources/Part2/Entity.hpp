#pragma once

#include "Engine/Color.hpp"
#include "Engine/Vector2.hpp"

class Entity
{
public:
	explicit Entity(char ch, Color color = 0xFFFFFF);

	char getChar() const;
	Color getColor() const;

	Vec2i getPosition() const;
	void setPosition(int x, int y);
	void move(int dx, int dy);

private:
	char m_ch;
	Color m_color;
	Vec2i m_position;
};
