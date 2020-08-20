#pragma once

#include "Menu.hpp"
#include "Engine/Vector2.hpp"

#include <vector>

class World;
class Actor;
class Item;
class Console;

class TargetingMenu : public Menu
{
public:
	TargetingMenu(World& world, Actor& actor, Item& item);

	void handleKeys(SDL_Keycode key) override;
	void draw(Console& console) override;

private:
	Vec2i getCursor() const;
	void setCursor(int x, int y);
	void setCursor(const Vec2i& position);
	void moveCursor(int dx, int dy);

private:
	World& m_world;
	Actor& m_actor;
	Item& m_item;
	Vec2i m_cursor;
	std::vector<Vec2i> m_path;
};
