#pragma once

#include "Engine/Vector2.hpp"
#include "Engine/Serializable.hpp"

#include <string>
#include <deque>

class Console;
class Actor;

class Panel : public Serializable
{
public:
	Panel(int x, int y, int width, int height);

	void setPlayer(Actor* player);
	void addMessage(std::string&& message);

	void draw(Console& console);

	void save(std::ostream& os) override;
	void load(std::istream& is) override;

private:
	Actor* m_player = nullptr;
	Vec2i m_position;
	Vec2i m_size;
	std::deque<std::string> m_messages;
};
