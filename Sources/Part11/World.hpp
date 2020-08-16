#pragma once

#include "Map.hpp"
#include "Level.hpp"
#include "Panel.hpp"
#include "Entity/Actor.hpp"
#include "Entity/Item.hpp"
#include "Menu/Menu.hpp"
#include "Engine/Fov.hpp"
#include "Engine/AStar.hpp"
#include "Engine/Serializable.hpp"

#include <memory>
#include <string>

class Console;

enum class GameState
{
	PlayerTurn,
	EnemyTurn,
	PlayerDead,
};

class World : public Serializable
{
public:
	World(int screenWidth, int screenHeight);

	void createLevel(unsigned int seed, bool placePlayerActor);

	GameState getGameState() const;
	Menu* getMenu() const;

	void setCurrentLevel(Level& level);

	// Actions
	void movePlayer(int dx, int dy);
	void waitPlayer();
	void pickUpItem();
	void checkStairs();

	void useItem(Item& item);
	void dropItem(Item::Ptr item);
	void throwItem(Item& item, std::vector<Vec2i>& path);

	void openInventory(SDL_Keycode key);
	void openTargeting(Item& item);
	void openLevelUpMenu();
	void closeMenu();

	void update(Console& console);

	bool isInBounds(const Vec2i& position) const;
	bool isPassable(const Vec2i& position) const;
	std::vector<Vec2i> findPath(const Vec2i& start, const Vec2i& target);

	Actor* getPlayerActor() const;
	Actor* getActor(const Vec2i& position);
	Item* getItem(const Vec2i& position);

	void openDoor(const Vec2i& position);
	void closeDoor(const Vec2i& position);

	void addMessage(std::string&& message);
	void markRemoveWrecks();

	void save(std::ostream& os) override;
	void load(std::istream& is) override;

private:
	void recomputeFov();
	void removeWrecks();
	void updateConsole(Console& console);

private:
	static constexpr int m_fovRange = 10;
	int m_mapWidth = 0;
	int m_mapHeight = 0;

	GameState m_gameState = GameState::PlayerTurn;
	std::vector<std::unique_ptr<Level>> m_levels;
	std::vector<std::unique_ptr<Actor>>* m_actors = nullptr;
	std::vector<std::unique_ptr<Item>>* m_items = nullptr;
	std::unique_ptr<Fov> m_fov = nullptr;
	std::unique_ptr<AStar> m_aStar = nullptr;
	std::unique_ptr<Panel> m_panel = nullptr;
	std::unique_ptr<Menu> m_menu = nullptr; // TODO: Move to Game class
	Level* m_level = nullptr;
	Map* m_map = nullptr;
	Actor* m_player = nullptr;
	bool m_needsFovUpdate = true;
	bool m_removeWrecks = false;
};
