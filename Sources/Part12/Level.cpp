#include "Level.hpp"
#include "Engine/Rng.hpp"
#include "Entity/Player.hpp"

Actor* Level::createMap(int width, int height, unsigned int seed, int depth)
{
	map = std::make_unique<Map>(width, height);
	explored.resize(width * height, false);

	this->seed = seed;
	this->depth = depth;
	Rng rng(seed);

	const auto rooms = generateDungeon(*map, rng);

	// Place entities
	Actor* player = nullptr;
	bool placePlayerActor = depth == 1;

	const std::vector<std::pair<int, int>> maxActorsTable =
	{
		{ 1, 2 },
		{ 4, 3 },
		{ 6, 5 },
	};

	const std::vector<std::pair<int, int>> maxItemsTable =
	{
		{ 1, 1 },
		{ 4, 2 },
	};

	const std::vector<std::pair<std::string, int>> actorsTable =
	{
		{ "orc", 80 },
		{ "troll", [this] () { return fromDepth({ {3, 15}, {5, 30}, {7, 60} }); }() },
	};

	const std::vector<std::pair<std::string, int>> itemsTable =
	{
		{ "potion of healing", 70 },
		{ "potion of confusion", 30 },
	};

	const int maxActorsPerRoom = fromDepth(maxActorsTable);
	const int maxItemsPerRoom = fromDepth(maxItemsTable);

	for (std::size_t i = 0; i < rooms.size(); ++i)
	{
		std::vector<Vec2i> positions;

		for (int y = rooms[i].y + 1; y <= rooms[i].y + rooms[i].height - 1; ++y)
			for (int x = rooms[i].x + 1; x <= rooms[i].x + rooms[i].width - 1; ++x)
				positions.emplace_back(x, y);

		if (placePlayerActor)
		{
			const auto pos = rng.pickOne(positions);
			auto actor = std::make_unique<Player>();
			actor->setPosition(pos);
			player = actor.get();
			actors.push_back(std::move(actor));
			placePlayerActor = false;
		}

		else
		{
			// Place actors
			rng.shuffle(positions);

			const int numActors = rng.getInt(0, maxActorsPerRoom);

			for (int j = 0; j < numActors; ++j)
			{
				const auto& id = rng.pickOneWeighted(actorsTable);
				auto actor = std::make_unique<Actor>(id);
				actor->setPosition(positions[j]);
				actors.push_back(std::move(actor));
			}

			// Place items
			rng.shuffle(positions);

			const int numItems = rng.getInt(0, maxItemsPerRoom);

			for (int j = 0; j < numItems; ++j)
			{
				const auto& id = rng.pickOneWeighted(itemsTable);
				auto item = std::make_unique<Item>(id);
				item->setPosition(positions[j]);
				items.push_back(std::move(item));
			}

			// Place up/down stairs
			if (i == 0 && depth > 1)
			{
				stairs.push_back({ '<', positions[numItems], nullptr });
				placeStairs(stairs.back());
			}

			else if (i == 1)
			{
				stairs.push_back({ '>', positions[numItems + 1], nullptr });
				placeStairs(stairs.back());
			}
		}
	}

	return player;
}

int Level::fromDepth(const std::vector<std::pair<int, int>>& table)
{
	for (std::size_t i = table.size(); i--;)
	{
		if (depth >= table[i].first)
			return table[i].second;
	}

	return 0;
}

void Level::placeStairs(const Stairs& stairs)
{
	map->at(stairs.position).ch = stairs.ch;
	map->at(stairs.position).color = stairs.color;
}

void Level::save(std::ostream& os)
{
	serialize(os, map->getWidth());
	serialize(os, map->getHeight());
	serialize(os, seed);
	serialize(os, depth);

	const std::size_t numActors = actors.size();
	serialize(os, numActors);

	for (std::size_t i = 0; i < numActors; ++i)
		actors[i]->save(os);

	const std::size_t numItems = items.size();
	serialize(os, numItems);

	for (std::size_t i = 0; i < numItems; ++i)
		items[i]->save(os);

	serialize(os, explored);
}

void Level::load(std::istream& is)
{
	int width;
	int height;

	deserialize(is, width);
	deserialize(is, height);
	deserialize(is, seed);
	deserialize(is, depth);

	map = std::make_unique<Map>(width, height);

	Rng rng(seed);

	generateDungeon(*map, rng);

	std::size_t numActors;
	deserialize(is, numActors);

	actors.resize(numActors);
	for (std::size_t i = 0; i < numActors; ++i)
	{
		std::string name;
		deserialize(is, name);
		if (name == "you")
			actors[i] = std::make_unique<Player>();
		else
			actors[i] = std::make_unique<Actor>(name);
		actors[i]->load(is);
	}

	std::size_t numItems;
	deserialize(is, numItems);

	items.resize(numItems);
	for (std::size_t i = 0; i < numItems; ++i)
	{
		std::string name;
		deserialize(is, name);
		items[i] = std::make_unique<Item>(name);
		items[i]->load(is);
	}

	deserialize(is, explored);
}
