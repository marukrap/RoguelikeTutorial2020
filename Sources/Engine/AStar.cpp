#include "AStar.hpp"
#include "Direction.hpp"

AStar::AStar(int width, int height, PassableFunction isPassable)
	: m_width(width)
	, m_height(height)
	, m_maxCost(std::numeric_limits<decltype(m_maxCost)>::max())
	, m_cells(width* height)
	, m_isPassable(std::move(isPassable))
	, m_heuristic(&Heuristic::roguelike)
{
}

void AStar::setMaxCost(std::size_t maxCost)
{
	m_maxCost = maxCost * 10;
}

void AStar::setHeuristic(HeuristicFunction heuristic)
{
	m_heuristic = std::move(heuristic);
}

std::vector<Vec2i> AStar::findPath(const Vec2i& start, const Vec2i& end)
{
	static const auto& directions = Direction::All;
	static constexpr std::array<int, 8> directionCost =
	{
		14, 10, 14,
		10,     10,
		14, 10, 14,
	};

	clear();
	m_openSet.push({ start, 0 });
	cell(start).cost = 0;

	while (!m_openSet.empty())
	{
		Vec2i current = m_openSet.top().position;
		m_openSet.pop();

		if (current == end)
		{
			std::vector<Vec2i> path;

			while (current != start)
			{
				path.emplace_back(current);
				current = cell(current).parent;
			}

			path.emplace_back(start);
			//std::reverse(path.begin(), path.end());

			return path;
		}

		Cell& currentCell = cell(current);
		currentCell.visited = true;

		for (std::size_t i = 0; i < directions.size(); ++i)
		{
			const Vec2i next = current + directions[i];

			if (!isInBounds(next) || !m_isPassable(next))
				continue;

			Cell& nextCell = cell(next);

			if (nextCell.visited)
				continue;

			const std::size_t newCost = currentCell.cost + directionCost[i];

			if (newCost < nextCell.cost)
			{
				m_openSet.push({ next, newCost + m_heuristic(next, end) });
				nextCell.cost = newCost;
				nextCell.parent = current;
			}
		}
	}

	return {};
}

void AStar::clear()
{
	m_openSet = decltype(m_openSet)();
	std::fill(m_cells.begin(), m_cells.end(), Cell{ {}, m_maxCost, false });
}

bool AStar::isInBounds(const Vec2i& position) const
{
	return position.x >= 0 && position.x < m_width && position.y >= 0 && position.y < m_height;
}
