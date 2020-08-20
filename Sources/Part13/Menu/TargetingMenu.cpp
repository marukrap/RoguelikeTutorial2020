#include "TargetingMenu.hpp"
#include "World.hpp"
#include "Engine/Console.hpp"

namespace
{
	constexpr int sign(int value)
	{
		return (value > 0) - (value < 0);
	}

	std::vector<Vec2i> plotLine(const Vec2i& start, const Vec2i& end)
	{
		const Vec2i delta = end - start;

		Vec2i primaryStep(sign(delta.x), 0);
		Vec2i secondaryStep(0, sign(delta.y));

		int primary = std::abs(delta.x);
		int secondary = std::abs(delta.y);

		if (secondary > primary)
		{
			std::swap(primary, secondary);
			std::swap(primaryStep, secondaryStep);
		}

		std::vector<Vec2i> line;
		Vec2i current = start;
		int error = 0;

		while (true)
		{
			line.emplace_back(current);

			if (current == end)
				break;

			current += primaryStep;
			error += secondary;

			if (error * 2 >= primary)
			{
				current += secondaryStep;
				error -= primary;
			}
		}

		return line;
	}
}

TargetingMenu::TargetingMenu(World& world, Actor& actor, Item& item)
	: m_world(world)
	, m_actor(actor)
	, m_item(item)
{
	setCursor(actor.getPosition());
}

Vec2i TargetingMenu::getCursor() const
{
	return m_cursor;
}

void TargetingMenu::setCursor(int x, int y)
{
	m_cursor.x = x;
	m_cursor.y = y;
	m_path = plotLine(m_actor.getPosition(), m_cursor);
}

void TargetingMenu::setCursor(const Vec2i& position)
{
	setCursor(position.x, position.y);
}

void TargetingMenu::moveCursor(int dx, int dy)
{
	setCursor(m_cursor.x + dx, m_cursor.y + dy);
}

void TargetingMenu::handleKeys(SDL_Keycode key)
{
	int dx = 0;
	int dy = 0;

	switch (key)
	{
	case SDLK_UP:
	case SDLK_KP_8:
	case SDLK_k:
		--dy;
		break;

	case SDLK_DOWN:
	case SDLK_KP_2:
	case SDLK_j:
		++dy;
		break;

	case SDLK_LEFT:
	case SDLK_KP_4:
	case SDLK_h:
		--dx;
		break;

	case SDLK_RIGHT:
	case SDLK_KP_6:
	case SDLK_l:
		++dx;
		break;

	case SDLK_HOME:
	case SDLK_KP_7:
	case SDLK_y:
		--dx;
		--dy;
		break;

	case SDLK_PAGEUP:
	case SDLK_KP_9:
	case SDLK_u:
		++dx;
		--dy;
		break;

	case SDLK_END:
	case SDLK_KP_1:
	case SDLK_b:
		--dx;
		++dy;
		break;

	case SDLK_PAGEDOWN:
	case SDLK_KP_3:
	case SDLK_n:
		++dx;
		++dy;
		break;

	case SDLK_INSERT:
	case SDLK_KP_0:
	case SDLK_ESCAPE:
		m_world.closeMenu();
		break;

	case SDLK_RETURN:
	case SDLK_SPACE:
	case SDLK_KP_ENTER:
	case SDLK_t:
		if (!m_path.empty())
		{
			m_world.throwItem(m_item, m_path);
			m_world.closeMenu();
		}
		break;
	}

	if (dx != 0 || dy != 0)
	{
		const Vec2i newPos = getCursor() + Vec2i(dx, dy);

		if (m_world.isInBounds(newPos))
			moveCursor(dx, dy);
	}
}

void TargetingMenu::draw(Console& console)
{
	console.drawBox(m_cursor.x - 1, m_cursor.y - 1, 3, 3);

	for (std::size_t i = 1; i + 1 < m_path.size(); ++i)
		console.setChar(m_path[i].x, m_path[i].y, '*');
}
