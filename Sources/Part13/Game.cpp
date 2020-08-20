#include "Game.hpp"
#include "Engine/OpenGL.hpp"
#include "Engine/Console.hpp"
#include "Engine/Rng.hpp"
#include "Menu/MainMenu.hpp"
#include "Menu/PauseMenu.hpp"
#include "Menu/InventoryMenu.hpp"

#include <filesystem>
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace
{
	Game* TheGame = nullptr;
	std::string Savepath;
	bool Saving = false;

	void removeSave()
	{
		std::filesystem::remove(Savepath);

#ifdef __EMSCRIPTEN__
		EM_ASM(
			FS.syncfs(function(err) {
				assert(!err);
			});
		);
#endif
	}
}

extern "C"
{
	void initFS()
	{
		TheGame->openMainMenu();
	}

	void onSave()
	{
		Saving = false;

		if (TheGame->isRunning() && !TheGame->getWorld())
			TheGame->openMainMenu();
	}
}

Game::Game(SDL_Window& window, Console& console)
	: m_window(window)
	, m_console(console)
	, m_renderer(console.getAtlas())
{
	glClearColor(0.f, 0.f, 0.f, 1.f);

	TheGame = this;
	Savepath = "Part13/Savefile";

#ifdef __EMSCRIPTEN__
	EM_ASM(
		FS.mkdir('/Part13');
		FS.mount(IDBFS, {}, '/Part13');

		FS.syncfs(true, function(err) {
			assert(!err);
			ccall('initFS')
		});
	);
#else
	initFS();
#endif
}

bool Game::isRunning() const
{
	return m_running;
}

void Game::tick()
{
	processInput();
	update();
	render();
}

World* Game::getWorld()
{
	return m_world.get();
}

void Game::createWorld()
{
	Rng rng;
	m_world = std::make_unique<World>(*this, m_console.getWidth(), m_console.getHeight());
	m_world->createLevel(rng.getSeed());
	removeSave();
}

void Game::loadSavefile()
{
	std::ifstream ifs(Savepath, std::ios::binary);

	if (!ifs)
	{
		std::cout << "Error: Unable to open savefile.\n";
		return;
	}

	m_world = std::make_unique<World>(*this, m_console.getWidth(), m_console.getHeight());
	m_world->load(ifs);
	ifs.close();
	removeSave();
}

void Game::closeMenu()
{
	m_menu = nullptr;
}

void Game::openMenu(std::unique_ptr<Menu> menu)
{
	m_menu = std::move(menu);
}

void Game::openMainMenu()
{
	const bool hasSavefile = std::filesystem::exists(Savepath);
	m_menu = std::make_unique<MainMenu>(*this, hasSavefile);
}

void Game::openPauseMenu()
{
	m_menu = std::make_unique<PauseMenu>(*this);
}

bool Game::isSaving()
{
	return Saving;
}

void Game::save(bool quit)
{
	if (m_world && m_world->getPlayerActor())
	{
		std::filesystem::create_directory("Part13");
		std::ofstream ofs(Savepath, std::ios::binary);

		if (!ofs)
		{
			std::cout << "Error: Unable to create savefile.\n";
			return;
		}

		m_world->save(ofs);
		ofs.close();
		Saving = true;
	}

	if (quit)
	{
		m_world = nullptr;
		m_menu = nullptr;
	}

#ifdef __EMSCRIPTEN__
	EM_ASM(
		FS.syncfs(function(err) {
		assert(!err);
		ccall('onSave')
	});
	);
#else
	onSave();
#endif
}

void Game::openInventory(SDL_Keycode key)
{
	if (m_world->getGameState() == GameState::PlayerTurn)
	{
		Actor* actor = m_world->getPlayerActor();

		if (actor->getInventory()->isEmpty())
			m_world->addMessage("your inventory is empty.");

		else
		{
			auto inventoryMenu = std::make_unique<InventoryMenu>(*m_world, *actor->getInventory());

			if (key == SDLK_a)
				inventoryMenu->selectButton(InventoryMenu::Use);
			else if (key == SDLK_d)
				inventoryMenu->selectButton(InventoryMenu::Drop);
			else if (key == SDLK_t)
				inventoryMenu->selectButton(InventoryMenu::Throw);

			m_menu = std::move(inventoryMenu);
		}
	}
}

void Game::processInput()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			m_running = false;
			save(true);
		}

		else if (event.type == SDL_KEYDOWN)
		{
			if (m_menu)
			{
				m_menu->handleKeys(event.key.keysym.sym);
				return;
			}

			int dx = 0;
			int dy = 0;

#ifdef _DEBUG
			if (SDL_GetModState() & KMOD_SHIFT)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_2: // SDLK_AT
					m_world->openLevelUpMenu();
					return;

				case SDLK_3: // SDLK_HASH
					m_world->revealMap();
					break;

				case SDLK_COMMA: // SDLK_LESS
					m_world->ascend();
					return;

				case SDLK_PERIOD: // SDLK_GREATER
					m_world->descend();
					return;
				}
			}
#endif

			switch (event.key.keysym.sym)
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

			case SDLK_CLEAR:
			case SDLK_KP_5:
			case SDLK_PERIOD:
				m_world->waitPlayer();
				break;

			case SDLK_INSERT:
			case SDLK_KP_0:
			case SDLK_i:
			case SDLK_a: // Apply(Use)
			case SDLK_d: // Drop
			case SDLK_t: // Throw
				openInventory(event.key.keysym.sym);
				break;

			case SDLK_ESCAPE:
				openPauseMenu();
				break;
			}

			if (dx != 0 || dy != 0)
				m_world->movePlayer(dx, dy);
		}
	}
}

void Game::update()
{
	m_console.clear();

	if (m_world)
		m_world->update(m_console);

	if (m_menu)
		m_menu->draw(m_console);

	m_renderer.setSprites(m_console.getSprites());
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	m_renderer.render(&m_window);
	SDL_GL_SwapWindow(&m_window);
}
