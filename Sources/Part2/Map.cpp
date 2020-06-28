#include "Map.hpp"

Map::Map(int width, int height)
	: m_width(width)
	, m_height(height)
	, m_tiles(width * height)
{
}

int Map::getWidth() const
{
	return m_width;
}

int Map::getHeight() const
{
	return m_height;
}

bool Map::isInBounds(int x, int y) const
{
	return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

Tile& Map::at(int x, int y)
{
	return m_tiles[x + y * m_width];
}

const Tile& Map::at(int x, int y) const
{
	return m_tiles[x + y * m_width];
}
