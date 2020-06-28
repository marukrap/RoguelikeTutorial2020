#include "Map.hpp"
#include "Engine/Rng.hpp"

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

bool Map::isInBounds(const Vec2i& position) const
{
	return isInBounds(position.x, position.y);
}

Tile& Map::at(int x, int y)
{
	return m_tiles[x + y * m_width];
}

const Tile& Map::at(int x, int y) const
{
	return m_tiles[x + y * m_width];
}

Tile& Map::at(const Vec2i& position)
{
	return at(position.x, position.y);
}

const Tile& Map::at(const Vec2i& position) const
{
	return at(position.x, position.y);
}

std::vector<Room> generateDungeon(Map& map, Rng& rng)
{
	// Credit: https://gist.github.com/munificent/b1bcd969063da3e6c298be070a22b604
	// Robert Nystrom @munificentbob for Ginny 2008-2019

	const int width = map.getWidth();
	const int height = map.getHeight();

	std::vector<Room> rooms;

	const auto addRoom = [&] (int start)
	{
		const int w = rng.getInt(5, 14);
		const int h = rng.getInt(3, 8);

		const int left   = rng.getInt(width - w - 2);
		const int top    = rng.getInt(height - h - 2);
		const int right  = left + w + 2;
		const int bottom = top + h + 2;

		for (int y = top; y <= bottom; ++y)
			for (int x = left; x <= right; ++x)
				if (map.at(x, y).ch == '.')
					return;

		int doorCount = 0;
		int dx, dy;

		if (!start)
		{
			for (int y = top; y <= bottom; ++y)
				for (int x = left; x <= right; ++x)
				{
					const int s = x == left || x == right;
					const int t = y == top || y == bottom;
					if (s ^ t && map.at(x, y).ch == '#')
					{
						if (rng.getInt(++doorCount) == 0)
						{
							dx = x;
							dy = y;
						}
					}
				}

			if (doorCount == 0)
				return;
		}

		for (int y = top; y <= bottom; ++y)
			for (int x = left; x <= right; ++x)
			{
				const int s = x == left || x == right;
				const int t = y == top || y == bottom;
				map.at(x, y).ch = s && t ? '!' : s ^ t ? '#' : '.';
			}

		if (doorCount > 0)
			map.at(dx, dy).ch = '+';

		rooms.push_back({ left, top, right - left, bottom - top });
	};

	for (int i = 0; i < width * height; ++i)
		addRoom(i == 0);

	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		{
			Tile& tile = map.at(x, y);

			if (tile.ch == '!')
				tile.ch = '#';

			switch (tile.ch)
			{
			case '#':
				tile.passable = false;
				tile.transparent = false;
				break;

			case '.':
				tile.passable = true;
				tile.transparent = true;
				break;

			case '+':
				tile.passable = true;
				tile.transparent = false;
				break;
			}
		}

	return rooms;
}
