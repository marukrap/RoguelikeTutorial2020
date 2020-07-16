#include "World.hpp"
#include "Engine/Console.hpp"
#include "Engine/Rng.hpp"

#include <algorithm> // find_if, remove_if
#include <functional> // mem_fn

World::World(int screenWidth, int screenHeight)
{
	const int panelWidth = screenWidth;
	const int panelHeight = 5;

	const int mapWidth = screenWidth;
	const int mapHeight = screenHeight - panelHeight;

	Entity::setWorld(*this);

	m_map = std::make_unique<Map>(mapWidth, mapHeight);
	Rng rng;

	const auto rooms = generateDungeon(*m_map, rng);

	// Place entities
	for (std::size_t i = 0; i < rooms.size(); ++i)
	{
		std::vector<Vec2i> positions;

		for (int y = rooms[i].y + 1; y <= rooms[i].y + rooms[i].height - 1; ++y)
			for (int x = rooms[i].x + 1; x <= rooms[i].x + rooms[i].width - 1; ++x)
				positions.emplace_back(x, y);

		rng.shuffle(positions);

		const int numActors = i == 0 ? 1 : rng.getInt(0, 3);

		for (int j = 0; j < numActors; ++j)
		{
			std::unique_ptr<Actor> actor;

			if (i == 0)
			{
				actor = std::make_unique<Actor>("you");
				m_player = actor.get();
			}

			else
				actor = std::make_unique<Actor>(rng.getInt(100) < 80 ? "orc" : "troll");

			actor->setPosition(positions[j]);
			m_actors.push_back(std::move(actor));
		}

		rng.shuffle(positions);

		const int numItems = i == 0 ? 0 : rng.getInt(0, 2);

		for (int j = 0; j < numItems; ++j)
		{
			auto item = std::make_unique<Item>("potion of healing");
			item->setPosition(positions[j]);
			m_items.push_back(std::move(item));
		}
	}

	m_fov = std::make_unique<Fov>(mapWidth, mapHeight, [this] (const Vec2i& pos) { return !m_map->at(pos).transparent; });
	m_aStar = std::make_unique<AStar>(mapWidth, mapHeight, [this] (const Vec2i& pos) { return m_map->at(pos).passable; });
	m_aStar->setMaxCost(25);

	m_panel = std::make_unique<Panel>(0, mapHeight, panelWidth, panelHeight);
	m_panel->setPlayer(m_player);
}

GameState World::getGameState() const
{
	return m_gameState;
}

void World::movePlayer(int dx, int dy)
{
	if (m_gameState != GameState::PlayerTurn)
		return;

	const Vec2i newPos = m_player->getPosition() + Vec2i(dx, dy);

	if (!m_map->isInBounds(newPos))
		return;

	if (Actor* actor = getActor(newPos))
	{
		m_player->attack(*actor);

		if (actor->isDestroyed())
			m_removeWrecks = true;
	}

	else if (m_map->at(newPos).passable)
	{
		m_player->move(dx, dy);
		m_needsFovUpdate = true;

		pickUpItem();
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
			const auto found = std::find_if(m_items.begin(), m_items.end(),
				[&] (const auto& i) { return i.get() == item; });

			m_panel->addMessage("you picked up " + (*found)->getAName() + ".");

			inventory->pack(std::move(*found));
			m_items.erase(found);
		}

		else
			m_panel->addMessage("your inventory is full.");
	}
}

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
	m_items.push_back(std::move(item));

	if (itemOnFloor)
	{
		const auto found = std::find_if(m_items.begin(), m_items.end(),
			[&] (const auto& i) { return i.get() == itemOnFloor; });

		m_panel->addMessage("you picked up " + (*found)->getAName() + ".");

		m_player->getInventory()->pack(std::move(*found));
		m_items.erase(found);
	}

	m_gameState = GameState::EnemyTurn;
}

void World::throwItem(Item::Ptr item)
{
	// TODO: ranged attack
	m_gameState = GameState::PlayerTurn;
}

void World::openInventory(SDL_Keycode key)
{
	if (m_gameState == GameState::PlayerTurn)
	{
		if (m_player->getInventory()->isEmpty())
			m_panel->addMessage("your inventory is empty.");

		else
		{
			m_gameState = GameState::ShowInventory;
			m_inventoryMenu = std::make_unique<InventoryMenu>(*m_player->getInventory());

			if (key == SDLK_a)
				m_inventoryMenu->selectButton(InventoryMenu::Use);
			else if (key == SDLK_d)
				m_inventoryMenu->selectButton(InventoryMenu::Drop);
			else if (key == SDLK_t)
				m_inventoryMenu->selectButton(InventoryMenu::Throw);
		}
	}
}

void World::closeInventory()
{
	m_inventoryMenu = nullptr;
}

void World::handleInventory(SDL_Keycode key)
{
	if (m_gameState != GameState::ShowInventory)
		return;

	switch (key)
	{
	case SDLK_UP:
	case SDLK_KP_8:
		m_inventoryMenu->selectPrevious();
		break;

	case SDLK_DOWN:
	case SDLK_KP_2:
		m_inventoryMenu->selectNext();
		break;

	case SDLK_LEFT:
	case SDLK_KP_4:
		m_inventoryMenu->selectPreviousButton();
		break;

	case SDLK_RIGHT:
	case SDLK_KP_6:
		m_inventoryMenu->selectNextButton();
		break;

	case SDLK_RETURN:
	case SDLK_SPACE:
	case SDLK_KP_ENTER:
		m_inventoryMenu->activateButton(*this);
		break;

	case SDLK_INSERT:
	case SDLK_KP_0:
	//case SDLK_i:
	case SDLK_ESCAPE:
		closeInventory();
		m_gameState = GameState::PlayerTurn;
		break;

	default:
		if (m_inventoryMenu->hasSelection())
		{
			if (key == SDLK_a)
			{
				m_inventoryMenu->selectButton(InventoryMenu::Use);
				m_inventoryMenu->activateButton(*this);
			}

			else if (key == SDLK_d)
			{
				m_inventoryMenu->selectButton(InventoryMenu::Drop);
				m_inventoryMenu->activateButton(*this);
			}

			else if (key == SDLK_t)
			{
				m_inventoryMenu->selectButton(InventoryMenu::Throw);
				m_inventoryMenu->activateButton(*this);
			}
		}

		else if (key >= SDLK_a && key <= SDLK_z)
		{
			if (m_inventoryMenu->hasButtonSelection())
			{
				m_inventoryMenu->select(key - SDLK_a);
				m_inventoryMenu->activateButton(*this);
			}

			else
				m_inventoryMenu->select(key - SDLK_a);
		}
		break;
	}
}

void World::update(Console& console)
{
	recomputeFov();

	if (m_gameState == GameState::EnemyTurn)
	{
		m_gameState = GameState::PlayerTurn;

		for (auto& actor : m_actors)
		{
			if (actor.get() == m_player || actor->isDestroyed())
				continue;

			if (!actor->getTarget() && m_fov->isVisible(actor->getPosition()))
				actor->setTarget(m_player);

			actor->updateAi();

			if (m_player->isDestroyed())
			{
				m_gameState = GameState::PlayerDead;
				m_player = nullptr;
				m_panel->setPlayer(nullptr);
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

Actor* World::getPlayerActor() const
{
	return m_player;
}

Actor* World::getActor(const Vec2i& position)
{
	const auto found = std::find_if(m_actors.begin(), m_actors.end(),
		[&] (const auto& a) { return a->getPosition() == position && !a->isDestroyed(); });

	if (found != m_actors.end())
		return found->get();

	return nullptr;
}

Item* World::getItem(const Vec2i& position)
{
	const auto found = std::find_if(m_items.begin(), m_items.end(),
		[&] (const auto& i) { return i->getPosition() == position; });

	if (found != m_items.end())
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
		m_actors.erase(std::remove_if(m_actors.begin(), m_actors.end(),
			std::mem_fn(&Actor::isDestroyed)), m_actors.end());

		m_removeWrecks = false;
	}
}

void World::updateConsole(Console& console)
{
	console.clear();

	// Draw map
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

	// Draw items
	for (const auto& item : m_items)
	{
		const Vec2i pos = item->getPosition();

		if (m_fov->isVisible(pos))
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

	// Draw actors
	for (const auto& actor : m_actors)
	{
		const Vec2i pos = actor->getPosition();

		if (m_fov->isVisible(pos))
			console.setChar(pos.x, pos.y, actor->getChar(), actor->getColor());
	}

	m_panel->draw(console);

	// Draw inventory
	if (m_gameState == GameState::ShowInventory)
		m_inventoryMenu->draw(console);
}
