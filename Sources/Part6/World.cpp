#include "World.hpp"
#include "Engine/Console.hpp"
#include "Engine/Rng.hpp"

#include <algorithm> // find_if, remove_if
#include <functional> // mem_fn

World::World(int mapWidth, int mapHeight)
{
	Entity::setWorld(*this);

	m_map = std::make_unique<Map>(mapWidth, mapHeight);
	Rng rng;

	const auto rooms = generateDungeon(*m_map, rng);

	// Place entities
	for (std::size_t i = 0; i < rooms.size(); ++i)
	{
		const int numEntities = i == 0 ? 1 : rng.getInt(0, 3);

		for (int j = 0; j < numEntities; ++j)
		{
			const int x = rng.getInt(rooms[i].x + 1, rooms[i].x + rooms[i].width - 1);
			const int y = rng.getInt(rooms[i].y + 1, rooms[i].y + rooms[i].height - 1);

			if (i == 0)
			{
				auto entity = std::make_unique<Entity>("player");
				entity->setPosition(x, y);
				m_player = entity.get();
				m_entities.push_back(std::move(entity));
			}

			else
			{
				auto entity = std::make_unique<Entity>(rng.getInt(100) < 80 ? "orc" : "troll");
				entity->setPosition(x, y);
				m_entities.push_back(std::move(entity));
			}
		}
	}

	m_fov = std::make_unique<Fov>(mapWidth, mapHeight, [this] (const Vec2i& pos) { return !m_map->at(pos).transparent; });
	m_aStar = std::make_unique<AStar>(mapWidth, mapHeight, [this] (const Vec2i& pos) { return m_map->at(pos).passable; });
	m_aStar->setMaxCost(25);
}

void World::movePlayer(int dx, int dy)
{
	if (m_gameState != GameState::PlayerTurn)
		return;

	const Vec2i newPos = m_player->getPosition() + Vec2i(dx, dy);

	if (!m_map->isInBounds(newPos))
		return;

	if (Entity* entity = getEntity(newPos))
	{
		m_log.clear();
		m_player->attack(*entity);

		if (entity->isDestroyed())
			m_removeWrecks = true;
	}

	else if (m_map->at(newPos).passable)
	{
		m_log.clear();
		m_player->move(dx, dy);
		m_needsFovUpdate = true;
	}

	m_gameState = GameState::EnemyTurn;
}

void World::waitPlayer()
{
	if (m_gameState == GameState::PlayerTurn)
	{
		m_log.clear();
		m_gameState = GameState::EnemyTurn;
	}
}

void World::update(Console& console)
{
	recomputeFov();

	if (m_gameState == GameState::EnemyTurn)
	{
		m_gameState = GameState::PlayerTurn;

		for (auto& entity : m_entities)
		{
			if (entity.get() == m_player || entity->isDestroyed())
				continue;

			if (!entity->getTarget() && m_fov->isVisible(entity->getPosition()))
				entity->setTarget(m_player);

			entity->updateAi();

			if (m_player->isDestroyed())
			{
				m_gameState = GameState::PlayerDead;
				m_removeWrecks = true;
				break;
			}
		}

		// Enemeies may have opened or closed doors.
		recomputeFov();
	}

	removeWrecks();
	updateConsole(console);
}

bool World::isPassable(const Vec2i& position) const
{
	return m_map->isInBounds(position) && m_map->at(position).passable;
}

std::vector<Vec2i> World::findPath(const Vec2i& start, const Vec2i& target)
{
	return m_aStar->findPath(start, target);
}

Entity* World::getEntity(const Vec2i& position)
{
	const auto found = std::find_if(m_entities.begin(), m_entities.end(),
		[&] (const auto& e) { return e->getPosition() == position && !e->isDestroyed(); });

	if (found != m_entities.end())
		return found->get();

	return nullptr;
}

Entity* World::getPlayerEntity() const
{
	return m_player;
}

void World::openDoor(const Vec2i& position)
{
	Tile& tile = m_map->at(position);

	if (tile.ch == '+' && !tile.transparent)
	{
		tile.transparent = true;
		m_needsFovUpdate = true;
	}
}

void World::closeDoor(const Vec2i& position)
{
	Tile& tile = m_map->at(position);

	if (tile.ch == '+' && tile.transparent)
	{
		tile.transparent = false;
		m_needsFovUpdate = true;
	}
}

void World::log(const std::string& log)
{
	if (!m_log.empty())
		m_log += ' ';

	m_log += log;
}

void World::recomputeFov()
{
	if (m_needsFovUpdate)
	{
		m_fov->clear();
		m_fov->compute(m_player->getPosition(), m_fovRange);
		m_needsFovUpdate = false;
	}
}

void World::removeWrecks()
{
	if (m_removeWrecks)
	{
		m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(),
			std::mem_fn(&Entity::isDestroyed)), m_entities.end());

		m_removeWrecks = false;
	}
}

void World::updateConsole(Console& console)
{
	console.clear();

	for (int y = 0; y < m_map->getHeight(); ++y)
		for (int x = 0; x < m_map->getWidth(); ++x)
		{
			const Tile& tile = m_map->at(x, y);

			if (m_fov->isVisible({ x, y }))
				console.setChar(x, y, tile.ch, tile.color);

			else if (m_fov->isExplored({ x, y }))
			{
				Color color = tile.color;
				color.r /= 5;
				color.g /= 5;
				color.b /= 5;
				console.setChar(x, y, tile.ch, color);
			}
		}

	for (const auto& entity : m_entities)
	{
		const Vec2i pos = entity->getPosition();

		if (m_fov->isVisible(pos))
			console.setChar(pos.x, pos.y, entity->getChar(), entity->getColor());
	}

	for (std::size_t i = 0; i < m_log.size(); ++i)
	{
		const int x = i % console.getWidth();
		const int y = i / console.getWidth();
		console.setChar(x, y, m_log[i]);
	}
}
