#include "Level.hpp"
#include "Engine/Rng.hpp"
#include "Entity/Player.hpp"

Actor* Level::createMap(int width, int height, unsigned int seed, bool placePlayerActor)
{
	map = std::make_unique<Map>(width, height);
	explored.resize(width * height, false);

	this->seed = seed;
	Rng rng(seed);

	const auto rooms = generateDungeon(*map, rng);

	// Place entities
	Actor* player = nullptr;

	for (std::size_t i = 0; i < rooms.size(); ++i)
	{
		std::vector<Vec2i> positions;

		for (int y = rooms[i].y + 1; y <= rooms[i].y + rooms[i].height - 1; ++y)
			for (int x = rooms[i].x + 1; x <= rooms[i].x + rooms[i].width - 1; ++x)
				positions.emplace_back(x, y);

		rng.shuffle(positions);

		const int numActors = placePlayerActor ? 1 : rng.getInt(0, 3);

		for (int j = 0; j < numActors; ++j)
		{
			std::unique_ptr<Actor> actor;

			if (placePlayerActor)
			{
				actor = std::make_unique<Player>();
				player = actor.get();
				placePlayerActor = false;
			}

			else
				actor = std::make_unique<Actor>(rng.getInt(100) < 80 ? "orc" : "troll");

			actor->setPosition(positions[j]);
			actors.push_back(std::move(actor));
		}

		rng.shuffle(positions);

		const int numItems = (player && i == 0) ? 0 : rng.getInt(0, 2);

		for (int j = 0; j < numItems; ++j)
		{
			std::string id;

			if (rng.getInt(2) == 0)
				id = "potion of healing";
			else
				id = "potion of confusion";

			auto item = std::make_unique<Item>(id);
			item->setPosition(positions[j]);
			items.push_back(std::move(item));
		}

		if (i == 0 && !player)
		{
			// Place up stairs
			stairs.push_back({ '<', positions[numItems], nullptr });
			placeStairs(stairs.back());
		}

		if (i == 1)
		{
			// Place down stairs
			stairs.push_back({ '>', positions[numItems + 1], nullptr });
			placeStairs(stairs.back());
		}
	}

	return player;
}

void Level::save(std::ostream& os)
{
	serialize(os, map->getWidth());
	serialize(os, map->getHeight());
	serialize(os, seed);

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

void Level::placeStairs(const Stairs& stairs)
{
	map->at(stairs.position).ch = stairs.ch;
	map->at(stairs.position).color = stairs.color;
}
