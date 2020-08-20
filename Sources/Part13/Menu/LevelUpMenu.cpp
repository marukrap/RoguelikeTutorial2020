#include "LevelUpMenu.hpp"
#include "World.hpp"
#include "Entity/Player.hpp"
#include "Engine/Console.hpp"

LevelUpMenu::LevelUpMenu(World& world)
	: m_world(world)
{
	const Actor* player = world.getPlayerActor();

	m_buttons.emplace_back("a) Constitution (+20 HP, from " + std::to_string(player->getMaxHp()) + ")");
	m_buttons.emplace_back("b) Strength (+1 attack, from " + std::to_string(player->getAttack()) + ")");
	m_buttons.emplace_back("c) Agility (+1 defense, from " + std::to_string(player->getDefense()) + ")");
}

void LevelUpMenu::handleKeys(SDL_Keycode key)
{
	switch (key)
	{
	case SDLK_UP:
	case SDLK_KP_8:
		selectPrevious();
		break;

	case SDLK_DOWN:
	case SDLK_KP_2:
		selectNext();
		break;

	case SDLK_RETURN:
	case SDLK_SPACE:
	case SDLK_KP_ENTER:
		activate();
		break;

	default:
		if (key >= SDLK_a && key <= SDLK_z)
		{
			select(key - SDLK_a);
			activate();
		}
		break;
	}
}

void LevelUpMenu::draw(Console& console)
{
	const std::string_view header = " Level Up! Choose a stat to raise ";

	std::size_t width = header.size() + 2;
	std::size_t height = m_buttons.size();

	for (const auto& button : m_buttons)
		width = std::max(width, button.size());

	const int x = (console.getWidth() - width) / 2;
	const int y = (console.getHeight() - height) / 2;

	console.clear(x - 2, y - 1, width + 4, height + 2);
	console.drawBox(x - 2, y - 1, width + 4, height + 2);

	for (std::size_t i = 0; i < m_buttons.size(); ++i)
	{
		Color color = 0xFFFFFF;

		console.setString(x, y + i, m_buttons[i], color);

		if (i == m_selected)
			console.setBgColor(x - 1, y + i, width + 2, 1, 0x3B1725);
	}

	console.setString(x + (width - header.size()) / 2, y - 1, header);
}

bool LevelUpMenu::hasSelection() const
{
	return m_selected >= 0;
}

void LevelUpMenu::select(std::size_t i)
{
	if (i < m_buttons.size())
		m_selected = i;
}

void LevelUpMenu::selectPrevious()
{
	m_selected = (m_selected + m_buttons.size() - 1) % m_buttons.size();
}

void LevelUpMenu::selectNext()
{
	m_selected = (m_selected + 1) % m_buttons.size();
}

void LevelUpMenu::activate()
{
	if (!hasSelection())
		return;

	auto* player = static_cast<Player*>(m_world.getPlayerActor());

	switch (m_selected)
	{
	case 0:
		player->increaseMaxHp(20);
		m_world.addMessage("you become healthier.");
		break;

	case 1:
		player->increaseAttack(1);
		m_world.addMessage("you become stronger.");
		break;

	case 2:
		player->increaseDefense(1);
		m_world.addMessage("you become more agile.");
		break;
	}

	m_world.closeMenu();
}
