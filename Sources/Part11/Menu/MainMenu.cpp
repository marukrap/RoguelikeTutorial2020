#include "MainMenu.hpp"
#include "World.hpp"
#include "Engine/Rng.hpp"
#include "Engine/Console.hpp"

#include <filesystem>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <sstream>

namespace
{
	std::stringstream iss;

	void onerror(void* arg)
	{
	}

	void ondelete(void* arg)
	{
	}

	void onload(void* arg, void* ptr, int num)
	{
		auto* iss = static_cast<std::stringstream*>(arg);
		iss->write(reinterpret_cast<const char*>(ptr), num);
	}

	void oncheck(void* arg, int exists)
	{
		if (exists)
		{
			auto* menu = static_cast<MainMenu*>(arg);
			menu->setHasSavefile();
			emscripten_idb_async_load("Part11", "Savefile", (void*)&iss, onload, onerror);
		}
	}
}
#endif

MainMenu::MainMenu(World& world)
	: m_world(world)
{
#ifdef __EMSCRIPTEN__
	emscripten_idb_async_exists("Part11", "Savefile", (void*)this, oncheck, onerror);

	m_buttons.emplace_back("New Game");
	m_callbacks.emplace_back([this] ()
	{
		Rng rng;
		m_world.createLevel(rng.getSeed(), true);

		if (m_hasSavefile)
			emscripten_idb_async_delete("Part11", "Savefile", 0, ondelete, onerror);
	});

	m_buttons.emplace_back("Continue");
	m_callbacks.emplace_back([this] ()
	{
		if (m_hasSavefile)
		{
			m_world.load(iss);
			emscripten_idb_async_delete("Part11", "Savefile", 0, ondelete, onerror);
		}
	});
#else
	if (std::filesystem::exists("Save/Savefile"))
	{
		m_hasSavefile = true;
		m_selected = 1;
	}

	m_buttons.emplace_back("New Game");
	m_callbacks.emplace_back([this] ()
	{
		Rng rng;
		m_world.createLevel(rng.getSeed(), true);

		if (m_hasSavefile)
			std::filesystem::remove("Save/Savefile");
	});

	m_buttons.emplace_back("Continue");
	m_callbacks.emplace_back([this] ()
	{
		if (m_hasSavefile)
		{
			std::ifstream ifs("Save/Savefile", std::ios::binary);
			m_world.load(ifs);
			ifs.close();
			std::filesystem::remove("Save/Savefile");
		}
	});
#endif
}

void MainMenu::setHasSavefile()
{
	m_hasSavefile = true;
	m_selected = 1;
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
		m_world.closeMenu();
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
