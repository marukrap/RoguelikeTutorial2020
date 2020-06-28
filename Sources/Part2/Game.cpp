#include "Game.hpp"
#include "Engine/OpenGL.hpp"
#include "Engine/Console.hpp"
#include "Engine/Rng.hpp"

Game::Game(SDL_Window& window, Console& console)
	: m_window(window)
	, m_console(console)
	, m_renderer(console.getAtlas())
{
	glClearColor(0.f, 0.f, 0.f, 1.f);

	m_map = std::make_unique<Map>(console.getWidth(), console.getHeight());
	Rng rng;

	do
	{
		for (int y = 0; y < m_map->getHeight(); ++y)
			for (int x = 0; x < m_map->getWidth(); ++x)
			{
				// Wall
				if (rng.getInt(4) == 0)
				{
					m_map->at(x, y).ch = '#';
					m_map->at(x, y).passable = false;
				}

				// Floor
				else
				{
					m_map->at(x, y).ch = '.';
					m_map->at(x, y).passable = true;

					if (!m_player)
					{
						auto entity = std::make_unique<Entity>('@');
						m_player = entity.get();
						m_player->setPosition(x, y);
						m_entities.push_back(std::move(entity));
					}
				}
			}
	} while (!m_player);
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

void Game::processInput()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
			m_running = false;

		else if (event.type == SDL_KEYDOWN)
		{
			int dx = 0;
			int dy = 0;

			switch (event.key.keysym.sym)
			{
			case SDLK_UP:
				--dy;
				break;

			case SDLK_DOWN:
				++dy;
				break;

			case SDLK_LEFT:
				--dx;
				break;

			case SDLK_RIGHT:
				++dx;
				break;
			}

			if (dx != 0 || dy != 0)
			{
				const Vec2i pos = m_player->getPosition() + Vec2i(dx, dy);

				if (m_map->isInBounds(pos.x, pos.y) && m_map->at(pos.x, pos.y).passable)
					m_player->move(dx, dy);
			}
		}
	}
}

void Game::update()
{
	m_console.clear();

	for (int y = 0; y < m_map->getHeight(); ++y)
		for (int x = 0; x < m_map->getWidth(); ++x)
		{
			const Tile& tile = m_map->at(x, y);
			m_console.setChar(x, y, tile.ch, tile.color);
		}

	for (const auto& entity : m_entities)
	{
		const Vec2i pos = entity->getPosition();
		m_console.setChar(pos.x, pos.y, entity->getChar(), entity->getColor());
	}

	m_renderer.setSprites(m_console.getSprites());
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	m_renderer.render(&m_window);
	SDL_GL_SwapWindow(&m_window);
}
