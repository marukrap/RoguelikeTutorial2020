#include "Fov.hpp"
#include "Map.hpp"

#include <algorithm> // min, max

Fov::Fov(Map& map)
	: m_map(&map)
{
}

void Fov::setMap(Map& map)
{
	m_map = &map;
}

void Fov::clear()
{
	for (int y = 0; y < m_map->getHeight(); ++y)
		for (int x = 0; x < m_map->getWidth(); ++x)
			m_map->at(x, y).visible = false;
}

void Fov::compute(const Vec2i& position, int range)
{
	if (range >= 0)
	{
		m_map->at(position).visible = true;
		m_map->at(position).explored = true;

		for (int octant = 0; octant < 8; ++octant)
			refreshOctant(octant, position, range);
	}
}

bool Fov::Shadow::contains(const Shadow& projection) const
{
	return start <= projection.start && end >= projection.end;
}

Fov::Shadow Fov::getProjection(int col, int row)
{
	const float topLeft = static_cast<float>(col) / (row + 2);
	const float bottomRight = static_cast<float>(col + 1) / (row + 1);

	return { topLeft, bottomRight };
}

bool Fov::isInShadow(const Shadow& projection) const
{
	for (const auto& shadow : m_shadows)
	{
		if (shadow.contains(projection))
			return true;
	}

	return false;
}

bool Fov::addShadow(const Shadow& shadow)
{
	std::size_t index = 0;

	for (; index < m_shadows.size(); ++index)
	{
		if (m_shadows[index].start > shadow.start)
			break;
	}

	const bool overlapsPrev = ((index > 0) && (m_shadows[index - 1].end > shadow.start));
	const bool overlapsNext = ((index < m_shadows.size()) && (m_shadows[index].start < shadow.end));

	if (overlapsNext)
	{
		if (overlapsPrev)
		{
			m_shadows[index - 1].end = std::max(m_shadows[index - 1].end, m_shadows[index].end);
			m_shadows.erase(m_shadows.begin() + index);
		}

		else
			m_shadows[index].start = std::min(m_shadows[index].start, shadow.start);
	}

	else
	{
		if (overlapsPrev)
			m_shadows[index - 1].end = std::max(m_shadows[index - 1].end, shadow.end);
		else
			m_shadows.emplace(m_shadows.begin() + index, shadow);
	}

	return (m_shadows.size() == 1) && (m_shadows[0].start == 0.f) && (m_shadows[0].end == 1.f);
}

void Fov::refreshOctant(int octant, const Vec2i& start, int range)
{
	Vec2i rowInc;
	Vec2i colInc;

	switch (octant)
	{
	case 0: rowInc = {  0, -1 }; colInc = {  1,  0 }; break;
	case 1: rowInc = {  1,  0 }; colInc = {  0, -1 }; break;
	case 2: rowInc = {  1,  0 }; colInc = {  0,  1 }; break;
	case 3: rowInc = {  0,  1 }; colInc = {  1,  0 }; break;
	case 4: rowInc = {  0,  1 }; colInc = { -1,  0 }; break;
	case 5: rowInc = { -1,  0 }; colInc = {  0,  1 }; break;
	case 6: rowInc = { -1,  0 }; colInc = {  0, -1 }; break;
	case 7: rowInc = {  0, -1 }; colInc = { -1,  0 }; break;
	}

	m_shadows.clear();

	bool fullShadow = false;

	for (int row = 1; row <= range; ++row)
	{
		Vec2i pos = start + (rowInc * row);

		if (!m_map->isInBounds(pos))
			break;

		for (int col = 0; col <= row; ++col)
		{
			// Circular field of view
			if ((pos - start).lengthSquared() > range * range)
				break;

			if (!fullShadow)
			{
				const Shadow projection = getProjection(col, row);

				if (!isInShadow(projection))
				{
					m_map->at(pos).visible = true;
					m_map->at(pos).explored = true;

					if (!m_map->at(pos).transparent)
						fullShadow = addShadow(projection);
				}
			}

			pos += colInc;

			if (!m_map->isInBounds(pos))
				break;
		}
	}
}
