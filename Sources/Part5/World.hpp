#pragma once

#include "Entity.hpp"
#include "Map.hpp"
#include "Fov.hpp"

#include <memory>
#include <string>

class Console;

class World
{
public:
	World(int mapWidth, int mapHeight);

	void movePlayer(int dx, int dy);
	void update(Console& console);

private:
	Entity* getEntity(const Vec2i& position);
	void removeWrecks();

private:
	static constexpr int m_fovRange = 10;

	std::unique_ptr<Map> m_map = nullptr;
	std::unique_ptr<Fov> m_fov = nullptr;
	std::vector<std::unique_ptr<Entity>> m_entities;
	Entity* m_player = nullptr;
	bool m_needsFovUpdate = true;
	bool m_removeWrecks = false;
	std::string m_log;
};
