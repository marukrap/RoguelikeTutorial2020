// Credit: http://journal.stuffwithstuff.com/2015/09/07/what-the-hero-sees/

#pragma once

#include "Vector2.hpp"

#include <functional>
#include <vector>

// Field of view
class Fov
{
public:
	using BlocksViewFunction = std::function<bool(Vec2i)>;

public:
	Fov(int width, int height, BlocksViewFunction blocksView);

	void clear();
	void compute(const Vec2i& position, int range);

	bool isVisible(const Vec2i& position) const;
	bool isExplroed(const Vec2i& position) const;

private:
	struct Shadow
	{
		float start, end;

		bool contains(const Shadow& projection) const;
	};

	Shadow getProjection(int col, int row);
	bool isInShadow(const Shadow& projection) const;
	bool addShadow(const Shadow& shadow);

	bool isInBounds(const Vec2i& position) const;
	void setVisible(const Vec2i& position, bool flag);
	void refreshOctant(int octant, const Vec2i& start, int range);

private:
	int m_width;
	int m_height;
	std::vector<bool> m_visible;
	std::vector<bool> m_explored;
	std::vector<Shadow> m_shadows;
	BlocksViewFunction m_blocksView;
};
