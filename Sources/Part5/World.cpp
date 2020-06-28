#include "World.hpp"
#include "Engine/Console.hpp"
#include "Engine/Rng.hpp"

#include <algorithm> // find_if, remove_if
#include <functional> // mem_fn

World::World(int mapWidth, int mapHeight)
{
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
			const char ch = i == 0 ? '@' : rng.getInt('A', 'Z');

			auto entity = std::make_unique<Entity>(ch);
			entity->setPosition(x, y);

			if (i == 0)
				m_player = entity.get();

			m_entities.push_back(std::move(entity));
		}
	}

	m_fov = std::make_unique<Fov>(*m_map);
}

void World::movePlayer(int dx, int dy)
{
	const Vec2i newPos = m_player->getPosition() + Vec2i(dx, dy);

	if (!m_map->isInBounds(newPos))
		return;

	if (Entity* entity = getEntity(newPos))
	{
		entity->takeDamage(1);

		if (entity->isDestroyed())
		{
			m_removeWrecks = true;
			m_log = "You defeated the ";
			m_log += entity->getChar();
			m_log += ". ";
		}
	}

	else if (m_map->at(newPos).passable)
	{
		m_player->move(dx, dy);
		m_needsFovUpdate = true;
		m_log = "";
	}
}

void World::update(Console& console)
{
	if (m_needsFovUpdate)
	{
		m_fov->clear();
		m_fov->compute(m_player->getPosition(), m_fovRange);
		m_needsFovUpdate = false;
	}

	if (m_removeWrecks)
	{
		removeWrecks();
		m_removeWrecks = false;
	}

	console.clear();

	for (int y = 0; y < m_map->getHeight(); ++y)
		for (int x = 0; x < m_map->getWidth(); ++x)
		{
			const Tile& tile = m_map->at(x, y);

			if (tile.visible)
				console.setChar(x, y, tile.ch, tile.color);

			else if (tile.explored)
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

		if (m_map->at(pos).visible)
			console.setChar(pos.x, pos.y, entity->getChar(), entity->getColor());
	}

	console.setString(0, 0, m_log);
}

Entity* World::getEntity(const Vec2i& position)
{
	const auto found = std::find_if(m_entities.begin(), m_entities.end(),
		[&] (const auto& e) { return e->getPosition() == position && !e->isDestroyed(); });

	if (found != m_entities.end())
		return found->get();

	return nullptr;
}

void World::removeWrecks()
{
	m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(),
		std::mem_fn(&Entity::isDestroyed)), m_entities.end());
}
