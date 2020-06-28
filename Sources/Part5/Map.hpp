#pragma once

#include "Engine/Color.hpp"
#include "Engine/Vector2.hpp"

#include <vector>

struct Tile
{
	char ch = ' ';
	Color color = 0xFFFFFF;
	bool passable = false;
	bool transparent = false;
};

class Map
{
public:
	Map(int width, int height);

	int getWidth() const;
	int getHeight() const;

	bool isInBounds(int x, int y) const;
	bool isInBounds(const Vec2i& position) const;

	Tile& at(int x, int y);
	const Tile& at(int x, int y) const;

	Tile& at(const Vec2i& position);
	const Tile& at(const Vec2i& position) const;

private:
	int m_width;
	int m_height;
	std::vector<Tile> m_tiles;
};

class Rng;

struct Room
{
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
};

std::vector<Room> generateDungeon(Map& map, Rng& rng);
