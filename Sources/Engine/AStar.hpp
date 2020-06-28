// Credit: https://github.com/Eurecat/astar-gridmap-2d

#pragma once

#include "Vector2.hpp"

#include <functional>
#include <queue>

class AStar
{
public:
	using HeuristicFunction = std::function<std::size_t(Vec2i, Vec2i)>;

	struct Heuristic
	{
		static std::size_t manhattan(const Vec2i& start, const Vec2i& end);
		static std::size_t euclidean(const Vec2i& start, const Vec2i& end);
		static std::size_t octagonal(const Vec2i& start, const Vec2i& end);
		static std::size_t roguelike(const Vec2i& start, const Vec2i& end);
	};

public:
	AStar(int width, int height, std::function<bool(Vec2i)> isPassable);

	void setMaxCost(std::size_t maxCost); // Max search depth
	void setHeuristic(HeuristicFunction heuristic);

	std::vector<Vec2i> findPath(const Vec2i& start, const Vec2i& end);

private:
	struct OpenNode
	{
		Vec2i position;
		std::size_t score;

		bool operator<(const OpenNode& b) const
		{
			return score > b.score;
		}
	};

	struct Cell
	{
		Vec2i parent;
		std::size_t cost;
		bool visited;
	};

	Cell& cell(const Vec2i& position);
	const Cell& cell(const Vec2i& position) const;

	void clear();
	bool isInBounds(const Vec2i& position) const;

private:
	int m_width;
	int m_height;
	std::vector<Cell> m_cells;
	std::priority_queue<OpenNode, std::vector<OpenNode>> m_openSet;
	std::size_t m_maxCost = std::numeric_limits<decltype(m_maxCost)>::max();
	std::function<bool(Vec2i)> isPassable;
	HeuristicFunction m_heuristic = &Heuristic::roguelike;
};

inline std::size_t AStar::Heuristic::manhattan(const Vec2i& start, const Vec2i& end)
{
	const Vec2i delta = end - start;
	return 10 * (std::abs(delta.x) + std::abs(delta.y));
}

inline std::size_t AStar::Heuristic::euclidean(const Vec2i& start, const Vec2i& end)
{
	const Vec2i delta = end - start;
	return static_cast<std::size_t>(10 * std::sqrt(delta.x * delta.x + delta.y * delta.y));
}

inline std::size_t AStar::Heuristic::octagonal(const Vec2i& start, const Vec2i& end)
{
	const Vec2i delta = { std::abs(end.x - start.x), std::abs(end.y - start.y) };
	return 10 * (delta.x + delta.y) - 6 * std::min(delta.x, delta.y);
}

inline std::size_t AStar::Heuristic::roguelike(const Vec2i& start, const Vec2i& end)
{
	const Vec2i delta = { std::abs(end.x - start.x), std::abs(end.y - start.y) };
	return 10 * std::max(delta.x, delta.y);
}

inline AStar::Cell& AStar::cell(const Vec2i& position)
{
	return m_cells[position.x + position.y * m_width];
}

inline const AStar::Cell& AStar::cell(const Vec2i& position) const
{
	return m_cells[position.x + position.y * m_width];
}
