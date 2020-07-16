#pragma once

#include "Actor.hpp"
#include "Item.hpp"
#include "Map.hpp"
#include "Panel.hpp"
#include "Engine/Fov.hpp"
#include "Engine/AStar.hpp"

#include <memory>
#include <string>

class Console;

enum class GameState
{
	PlayerTurn,
	EnemyTurn,
	PlayerDead,
	ShowInventory,
};

class World
{
public:
	World(int screenWidth, int screenHeight);

	GameState getGameState() const;

	// Actions
	void movePlayer(int dx, int dy);
	void waitPlayer();
	void pickUpItem();

	void useItem(Item& item);
	void dropItem(Item::Ptr item);
	void throwItem(Item::Ptr item);

	void openInventory(SDL_Keycode key);
	void closeInventory();
	void handleInventory(SDL_Keycode key);

	void update(Console& console);

	bool isPassable(const Vec2i& position) const;
	std::vector<Vec2i> findPath(const Vec2i& start, const Vec2i& target);

	Actor* getPlayerActor() const;
	Actor* getActor(const Vec2i& position);
	Item* getItem(const Vec2i& position);

	void openDoor(const Vec2i& position);
	void closeDoor(const Vec2i& position);

	void addMessage(std::string&& message);

private:
	void recomputeFov();
	void removeWrecks();
	void updateConsole(Console& console);

private:
	static constexpr int m_fovRange = 10;

	GameState m_gameState = GameState::PlayerTurn;
	std::unique_ptr<Map> m_map = nullptr;
	std::unique_ptr<Fov> m_fov = nullptr;
	std::unique_ptr<AStar> m_aStar = nullptr;
	std::unique_ptr<Panel> m_panel = nullptr;
	std::vector<std::unique_ptr<Actor>> m_actors;
	std::vector<std::unique_ptr<Item>> m_items;
	std::unique_ptr<InventoryMenu> m_inventoryMenu = nullptr;
	Actor* m_player = nullptr;
	bool m_needsFovUpdate = true;
	bool m_removeWrecks = false;
};
