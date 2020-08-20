#include "MainMenu.hpp"
#include "Game.hpp"
#include "Engine/Console.hpp"

MainMenu::MainMenu(Game& game, bool hasSavefile)
	: m_game(game)
	, m_hasSavefile(hasSavefile)
	, m_selected(hasSavefile ? 1 : 0)
{
	m_buttons.emplace_back("New Game");
	m_callbacks.emplace_back([this] ()
	{
		m_game.createWorld();
	});

	m_buttons.emplace_back("Continue");
	m_callbacks.emplace_back([this] ()
	{
		m_game.loadSavefile();
	});
}

void MainMenu::handleKeys(SDL_Keycode key)
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
		m_callbacks[m_selected]();
		m_game.closeMenu();
		break;
	}
}

void MainMenu::draw(Console& console)
{
	std::size_t width = 0;
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

		if (i == 1 && !m_hasSavefile)
			color = 0x333941;

		console.setString(x, y + i, m_buttons[i], color);

		if (i == m_selected)
			console.setBgColor(x - 1, y + i, width + 2, 1, 0x3B1725);
	}
}

void MainMenu::selectPrevious()
{
	m_selected = (m_selected + m_buttons.size() - 1) % m_buttons.size();

	if (m_selected == 1 && !m_hasSavefile)
		selectPrevious();
}

void MainMenu::selectNext()
{
	m_selected = (m_selected + 1) % m_buttons.size();

	if (m_selected == 1 && !m_hasSavefile)
		selectNext();
}
