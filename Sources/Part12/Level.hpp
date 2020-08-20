#pragma once

#include "Map.hpp"
#include "Entity/Actor.hpp"
#include "Entity/Item.hpp"
#include "Engine/Serializable.hpp"

#include <cassert>

struct Level;

struct Stairs
{
	static constexpr Color color = 0x20D6C7;

	char ch;
	Vec2i position;
	Level* destination = nullptr;
};

struct Level : public Serializable
{
public:
	Actor* createMap(int width, int height, unsigned int seed, int depth);

	int fromDepth(const std::vector<std::pair<int, int>>& table);
	void placeStairs(const Stairs& stairs);

	void save(std::ostream& os) override;
	void load(std::istream& is) override;

public:
	unsigned int seed = 0;
	int depth = 1;
	std::unique_ptr<Map> map = nullptr;
	std::vector<std::unique_ptr<Actor>> actors;
	std::vector<std::unique_ptr<Item>> items;
	std::vector<bool> explored;
	std::vector<Stairs> stairs;
};
