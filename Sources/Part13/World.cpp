#include "World.hpp"
#include "Game.hpp"
#include "Engine/Console.hpp"
#include "Engine/Rng.hpp"
#include "Entity/Player.hpp"
#include "Menu/TargetingMenu.hpp"
#include "Menu/LevelUpMenu.hpp"

#include <algorithm> // find_if, remove_if
#include <functional> // mem_fn

namespace
{
	constexpr int PanelHeight = 5;
}

World::World(Game& game, int screenWidth, int screenHeight)
	: m_game(game)
{
	m_mapWidth = screenWidth;
	m_mapHeight = screenHeight - PanelHeight;

	Entity::setWorld(*this);
}

void World::createLevel(unsigned int seed)
{
	const int depth = m_level ? m_level->depth + 1 : 1;

	auto level = std::make_unique<Level>();
	Actor* actor = level->createMap(m_mapWidth, m_mapHeight, seed, depth);
	if (actor)
		m_player = actor;

	setCurrentLevel(*level);
	m_levels.push_back(std::move(level));
}

void World::setCurrentLevel(Level& level)
{
	if (m_level)
	{
		const auto found = std::find_if(m_actors->begin(), m_actors->end(),
			[this] (const auto& a) { return a.get() == m_player; });

		level.actors.push_back(std::move(*found));
		m_actors->erase(found);
		m_fov->save(m_level->explored);
	}

	m_level = &level;
	m_map = level.map.get();
	m_actors = &level.actors;
	m_items = &level.items;

	if (!m_fov)
		m_fov = std::make_unique<Fov>(m_mapWidth, m_mapHeight, [this] (const Vec2i& pos) { return !m_map->at(pos).transparent; });

	m_fov->load(level.explored);

	if (!m_aStar)
	{
		m_aStar = std::make_unique<AStar>(m_mapWidth, m_mapHeight, [this] (const Vec2i& pos) { return m_map->at(pos).passable; });
		m_aStar->setMaxCost(25);
	}

	if (!m_panel)
	{
		m_panel = std::make_unique<Panel>(0, m_mapHeight, m_mapWidth, PanelHeight);
		m_panel->setPlayer(m_player);
	}
}

GameState World::getGameState() const
{
	return m_gameState;
}

void World::movePlayer(int dx, int dy)
{
	if (m_gameState != GameState::PlayerTurn)
		return;

	if (m_player->hasStatusEffect(StatusEffect::Confused))
	{
		do
		{
			dx = randomInt(-1, 1);
			dy = randomInt(-1, 1);
		} while (dx == 0 && dy == 0);
	}

	const Vec2i newPos = m_player->getPosition() + Vec2i(dx, dy);

	if (!m_map->isInBounds(newPos))
		return;

	if (Actor* actor = getActor(newPos))
		m_player->attack(*actor);

	else if (m_map->at(newPos).passable)
	{
		m_player->move(dx, dy);
		m_needsFovUpdate = true;

		pickUpItem();
		checkStairs();
	}

	m_gameState = GameState::EnemyTurn;
}

void World::waitPlayer()
{
	if (m_gameState == GameState::PlayerTurn)
		m_gameState = GameState::EnemyTurn;
}

void World::pickUpItem()
{
	if (Item* item = getItem(m_player->getPosition()))
	{
		Inventory* inventory = m_player->getInventory();

		if (!inventory->isFull())
		{
			const auto found = std::find_if(m_items->begin(), m_items->end(),
				[&] (const auto& i) { return i.get() == item; });

			m_panel->addMessage("you picked up " + (*found)->getAName() + ".");

			inventory->pack(std::move(*found));
			m_items->erase(found);
		}

		else
			m_panel->addMessage("your inventory is full.");
	}
}

void World::checkStairs()
{
	for (auto& stairs : m_level->stairs)
	{
		if (stairs.position == m_player->getPosition())
		{
			if (stairs.destination)
			{
				Level* prevLevel = m_level;
				setCurrentLevel(*stairs.destination);

				for (const auto& stairs2 : m_level->stairs)
				{
					if (stairs2.destination == prevLevel)
					{
						m_player->setPosition(stairs2.position);
						break;
					}
				}
			}

			else
			{
				Rng rng;
				Level* prevLevel = m_level;
				createLevel(rng.getSeed());
				stairs.destination = m_level;

				for (auto& stairs2 : m_level->stairs)
				{
					if (stairs.ch != stairs2.ch)
					{
						stairs2.destination = prevLevel;
						m_player->setPosition(stairs2.position);
						break;
					}
				}
			}

			break;
		}
	}
}

#ifdef _DEBUG
void World::revealMap()
{
	m_wizardVision = !m_wizardVision;
}

void World::ascend()
{
	for (const auto& stairs : m_level->stairs)
	{
		if (stairs.ch == '<' && stairs.destination)
		{
			Level* prevLevel = m_level;
			setCurrentLevel(*stairs.destination);

			for (const auto& stairs2 : m_level->stairs)
			{
				if (stairs2.destination == prevLevel)
				{
					m_player->setPosition(stairs2.position);
					break;
				}
			}

			m_needsFovUpdate = true;
		}
	}
}

void World::descend()
{
	for (auto& stairs : m_level->stairs)
	{
		if (stairs.ch == '>')
		{
			if (stairs.destination)
			{
				Level* prevLevel = m_level;
				setCurrentLevel(*stairs.destination);

				for (const auto& stairs2 : m_level->stairs)
				{
					if (stairs2.destination == prevLevel)
					{
						m_player->setPosition(stairs2.position);
						break;
					}
				}
			}

			else
			{
				Rng rng;
				Level* prevLevel = m_level;
				createLevel(rng.getSeed());
				stairs.destination = m_level;

				for (auto& stairs2 : m_level->stairs)
				{
					if (stairs.ch != stairs2.ch)
					{
						stairs2.destination = prevLevel;
						m_player->setPosition(stairs2.position);
						break;
					}
				}
			}

			m_needsFovUpdate = true;
		}
	}
}
#endif

void World::useItem(Item& item)
{
	item.apply(*m_player);
	m_gameState = GameState::EnemyTurn;
}

void World::dropItem(Item::Ptr item)
{
	Item* itemOnFloor = getItem(m_player->getPosition());

	m_panel->addMessage("you dropped " + item->getAName() + ".");
	item->setPosition(m_player->getPosition());
	m_items->push_back(std::move(item));

	if (itemOnFloor)
	{
		const auto found = std::find_if(m_items->begin(), m_items->end(),
			[&] (const auto& i) { return i.get() == itemOnFloor; });

		m_panel->addMessage("you picked up " + (*found)->getAName() + ".");

		m_player->getInventory()->pack(std::move(*found));
		m_items->erase(found);
	}

	m_gameState = GameState::EnemyTurn;
}

void World::throwItem(Item& item, std::vector<Vec2i>& path)
{
	auto itemPtr = m_player->getInventory()->unpack(item);

	for (std::size_t i = 1; i < path.size(); ++i)
	{
		itemPtr->setPosition(path[i]);

		if (Actor* actor = getActor(path[i]))
		{
			itemPtr->throwAt(*actor);
			break;
		}

		Tile& tile = m_map->at(path[i]);

		// isWall
		if (tile.ch == '#' || tile.ch == '+')
		{
			itemPtr->setPosition(path[i - 1]);
			break;
		}
	}

	if (itemPtr && itemPtr->getCount() > 0)
	{
		if (itemPtr->getChar() == '!')
			m_panel->addMessage("the flask shattered.");
		else
			m_items->push_back(std::move(itemPtr));
	}

	m_gameState = GameState::EnemyTurn;
}

void World::closeMenu()
{
	m_game.closeMenu();
}

void World::openTargeting(Item& item)
{
	auto menu = std::make_unique<TargetingMenu>(*this, *m_player, item);
	m_game.openMenu(std::move(menu));
}

void World::openLevelUpMenu()
{
	auto menu = std::make_unique<LevelUpMenu>(*this);
	m_game.openMenu(std::move(menu));
}

void World::update(Console& console)
{
	recomputeFov();

	if (m_gameState == GameState::EnemyTurn)
	{
		m_player->finishTurn();
		m_gameState = GameState::PlayerTurn;

		for (auto& actor : *m_actors)
		{
			if (actor.get() == m_player || actor->isDestroyed())
				continue;

			if (!actor->getTarget() && m_fov->isVisible(actor->getPosition()))
				actor->setTarget(m_player);

			actor->updateAi();
			actor->finishTurn();

			if (m_player->isDestroyed())
			{
				m_gameState = GameState::PlayerDead;
				m_player = nullptr;
				m_panel->setPlayer(nullptr);
				m_game.closeMenu(); // Close level up menu if you leveled up this turn.
				break;
			}
		}

		// Enemeies may have opened or closed doors.
		recomputeFov();
		removeWrecks();
	}

	updateConsole(console);
}

bool World::isInBounds(const Vec2i& position) const
{
	return m_map->isInBounds(position);
}

bool World::isPassable(const Vec2i& position) const
{
	return m_map->isInBounds(position) && m_map->at(position).passable;
}

std::vector<Vec2i> World::findPath(const Vec2i& start, const Vec2i& target)
{
	return m_aStar->findPath(start, target);
}

Actor* World::getPlayerActor() const
{
	return m_player;
}

Actor* World::getActor(const Vec2i& position)
{
	const auto found = std::find_if(m_actors->begin(), m_actors->end(),
		[&] (const auto& a) { return a->getPosition() == position && !a->isDestroyed(); });

	if (found != m_actors->end())
		return found->get();

	return nullptr;
}

Item* World::getItem(const Vec2i& position)
{
	const auto found = std::find_if(m_items->begin(), m_items->end(),
		[&] (const auto& i) { return i->getPosition() == position; });

	if (found != m_items->end())
		return found->get();

	return nullptr;
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

void World::addMessage(std::string&& message)
{
	m_panel->addMessage(std::move(message));
}

void World::markRemoveWrecks()
{
	m_removeWrecks = true;
}

void World::save(std::ostream& os)
{
	// TODO: Badly structured because of compatibility for the previous tutorials,
	//       Need to fix Map and Fov classes.

	const std::size_t numLevels = m_levels.size();
	serialize(os, numLevels);

	std::size_t levelId;
	std::size_t playerId;

	for (std::size_t i = 0; i < numLevels; ++i)
	{
		if (m_levels[i].get() == m_level)
		{
			levelId = i;

			for (std::size_t j = 0; j < m_levels[i]->actors.size(); ++j)
			{
				if (m_levels[i]->actors[j].get() == m_player)
					playerId = j;
			}

			m_fov->save(m_levels[i]->explored);
		}

		m_levels[i]->save(os);
	}

	// Serialize stairs
	for (std::size_t i = 0; i < numLevels; ++i)
	{
		const std::size_t numStairs = m_levels[i]->stairs.size();
		serialize(os, numStairs);

		for (std::size_t j = 0; j < numStairs; ++j)
		{
			const auto& stairs = m_levels[i]->stairs[j];

			serialize(os, stairs.ch);
			serialize(os, stairs.position);

			int destId = -1;

			if (stairs.destination)
			{
				for (std::size_t k = 0; k < m_levels.size(); ++k)
				{
					if (stairs.destination == m_levels[k].get())
					{
						destId = k;
						break;
					}
				}
			}

			serialize(os, destId);
		}
	}

	serialize(os, levelId);
	serialize(os, playerId);

	for (const auto& level : m_levels)
		for (const auto& actor : level->actors)
			serialize(os, actor->getTarget() != nullptr);

	m_panel->save(os);
}

void World::load(std::istream& is)
{
	std::size_t numLevels;
	deserialize(is, numLevels);

	m_levels.resize(numLevels);
	for (std::size_t i = 0; i < numLevels; ++i)
	{
		m_levels[i] = std::make_unique<Level>();
		m_levels[i]->load(is);
	}

	// Deserialize stairs
	for (std::size_t i = 0; i < numLevels; ++i)
	{
		std::size_t numStairs;
		deserialize(is, numStairs);

		m_levels[i]->stairs.resize(numStairs);
		for (std::size_t j = 0; j < numStairs; ++j)
		{
			auto& stairs = m_levels[i]->stairs[j];

			deserialize(is, stairs.ch);
			deserialize(is, stairs.position);

			int destId;
			deserialize(is, destId);

			if (destId >= 0)
				stairs.destination = m_levels[destId].get();

			m_levels[i]->placeStairs(stairs);
		}
	}

	std::size_t levelId;
	std::size_t playerId;

	deserialize(is, levelId);
	deserialize(is, playerId);

	m_player = m_levels[levelId]->actors[playerId].get();
	setCurrentLevel(*m_levels[levelId]);

	for (auto& level : m_levels)
		for (auto& actor : level->actors)
		{
			bool hadTarget;
			deserialize(is, hadTarget);

			if (hadTarget)
				actor->setTarget(m_player);
		}

	m_fov->load(m_levels[levelId]->explored);
	m_panel->load(is);
}

void World::recomputeFov()
{
	if (m_needsFovUpdate && m_player)
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
		m_actors->erase(std::remove_if(m_actors->begin(), m_actors->end(),
			std::mem_fn(&Actor::isDestroyed)), m_actors->end());

		m_removeWrecks = false;
	}
}

void World::updateConsole(Console& console)
{
	// Draw map
	if (m_map)
	{
		for (int y = 0; y < m_map->getHeight(); ++y)
			for (int x = 0; x < m_map->getWidth(); ++x)
			{
				const Tile& tile = m_map->at(x, y);

				if (m_wizardVision || m_fov->isVisible({ x, y }))
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
	}

	// Draw items
	if (m_items)
	{
		for (const auto& item : *m_items)
		{
			const Vec2i pos = item->getPosition();

			if (m_wizardVision || m_fov->isVisible(pos))
				console.setChar(pos.x, pos.y, item->getChar(), item->getColor());

			else if (m_fov->isExplored(pos))
			{
				Color color = item->getColor();
				color.r /= 5;
				color.g /= 5;
				color.b /= 5;
				console.setChar(pos.x, pos.y, item->getChar(), color);
			}
		}
	}

	// Draw actors
	if (m_actors)
	{
		for (const auto& actor : *m_actors)
		{
			const Vec2i pos = actor->getPosition();

			if (m_wizardVision || m_fov->isVisible(pos))
				console.setChar(pos.x, pos.y, actor->getChar(), actor->getColor());
		}
	}

	if (m_panel)
		m_panel->draw(console);

	const std::string depthText = "Depth " + std::to_string(m_level->depth);
	console.setString(0, 0, depthText);
}
