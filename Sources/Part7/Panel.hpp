#pragma once

#include "Engine/Vector2.hpp"

#include <string>
#include <deque>

class Console;
class Entity;

class Panel
{
public:
	Panel(int x, int y, int width, int height);

	void setPlayer(Entity* player);
	void addMessage(std::string&& message);

	void draw(Console& console);

private:
	Entity* m_player = nullptr;
	Vec2i m_position;
	Vec2i m_size;
	std::deque<std::string> m_messages;
};
