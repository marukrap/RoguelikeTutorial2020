// Credit: http://journal.stuffwithstuff.com/2015/09/07/what-the-hero-sees/

#pragma once

#include "Engine/Vector2.hpp"

#include <vector>

class Map;

// Field of view
class Fov
{
public:
	Fov() = default;
	explicit Fov(Map& map);

	void setMap(Map& map);

	void clear();
	void compute(const Vec2i& position, int range);

private:
	struct Shadow
	{
		float start, end;

		bool contains(const Shadow& projection) const;
	};

	Shadow getProjection(int col, int row);
	bool isInShadow(const Shadow& projection) const;
	bool addShadow(const Shadow& shadow);

	void refreshOctant(int octant, const Vec2i& start, int range);

private:
	Map* m_map = nullptr;
	std::vector<Shadow> m_shadows;
};
