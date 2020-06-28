#pragma once

#include "Vector2.hpp"

#include <array>

class Direction : public Vec2i
{
public:
	using Vec2i::Vector2;
	/* implicit */ Direction(const Vec2i& vector);

	Direction left45() const;
	Direction right45() const;

	Direction left90() const;
	Direction right90() const;

public:
	static const Direction None;
	static const Direction NW;
	static const Direction N;
	static const Direction NE;
	static const Direction W;
	static const Direction E;
	static const Direction SW;
	static const Direction S;
	static const Direction SE;

	static const std::array<Direction, 8> All;
	static const std::array<Direction, 4> Cardinal;
	static const std::array<Direction, 4> Diagonal;
};
