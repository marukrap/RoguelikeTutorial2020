#pragma once

#include "Engine/Color.hpp"
#include "Engine/Vector2.hpp"

#include <string>
#include <string_view>

class World;

class Entity
{
public:
	virtual ~Entity() = default;

	char getChar() const;
	Color getColor() const;

	virtual std::string_view getName() const = 0;
	virtual std::string getAName() const;
	virtual std::string getTheName() const;

	Vec2i getPosition() const;
	void setPosition(int x, int y);
	void setPosition(const Vec2i& position);
	void move(int dx, int dy);

	static void setWorld(World& world);

protected:
	static World* s_world;

	char m_ch;
	Color m_color;
	Vec2i m_position;
};
