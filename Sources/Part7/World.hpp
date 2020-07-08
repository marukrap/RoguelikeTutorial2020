#pragma once

#include "Entity.hpp"
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
};

class World
{
public:
	World(int screenWidth, int screenHeight);

	void movePlayer(int dx, int dy);
	void waitPlayer();

	void update(Console& console);

	bool isPassable(const Vec2i& position) const;
	std::vector<Vec2i> findPath(const Vec2i& start, const Vec2i& target);

	Entity* getEntity(const Vec2i& position);
	Entity* getPlayerEntity() const;

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
	std::vector<std::unique_ptr<Entity>> m_entities;
	Entity* m_player = nullptr;
	bool m_needsFovUpdate = true;
	bool m_removeWrecks = false;
};
