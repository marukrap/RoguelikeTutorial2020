#pragma once

#include "Engine/Color.hpp"
#include "Engine/Vector2.hpp"

#include <string>
#include <string_view>

class World;
struct EntityData;

class Entity
{
public:
	explicit Entity(std::string_view name);

	char getChar() const;
	Color getColor() const;

	std::string_view getName() const;
	std::string getTheName() const;

	Vec2i getPosition() const;
	void setPosition(int x, int y);
	void setPosition(const Vec2i& position);
	void move(int dx, int dy);

	bool isDestroyed() const;
	void takeDamage(int damage);
	void attack(Entity& target);

	Entity* getTarget() const;
	void setTarget(Entity* target);
	void updateAi();

	static void setWorld(World& world);

private:
	static World* s_world;

	const std::string_view m_name;
	const EntityData& m_data;

	char m_ch;
	Color m_color;
	Vec2i m_position;

	int m_hp;
	int m_attack;
	int m_defense;

	Entity* m_target = nullptr;
};
