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

	const int mapWidth = console.getWidth();
	const int mapHeight = console.getHeight();

	m_map = std::make_unique<Map>(mapWidth, mapHeight);
	Rng rng;

	const auto rooms = generateDungeon(*m_map, rng);

	// Create and place the player character
	const int x = rng.getInt(rooms[0].x + 1, rooms[0].x + rooms[0].width - 1);
	const int y = rng.getInt(rooms[0].y + 1, rooms[0].y + rooms[0].height - 1);
	auto entity = std::make_unique<Entity>('@');
	entity->setPosition(x, y);
	m_player = entity.get();
	m_entities.push_back(std::move(entity));

	m_fov = std::make_unique<Fov>(mapWidth, mapHeight, [this] (const Vec2i& pos) { return !m_map->at(pos).transparent; });
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

				if (m_map->isInBounds(pos) && m_map->at(pos).passable)
				{
					m_player->move(dx, dy);
					m_needsFovUpdate = true;
				}
			}
		}
	}
}

void Game::update()
{
	if (m_needsFovUpdate)
	{
		m_fov->clear();
		m_fov->compute(m_player->getPosition(), m_fovRange);
		m_needsFovUpdate = false;
	}

	m_console.clear();

	for (int y = 0; y < m_map->getHeight(); ++y)
		for (int x = 0; x < m_map->getWidth(); ++x)
		{
			const Tile& tile = m_map->at(x, y);

			if (m_fov->isVisible({ x, y }))
				m_console.setChar(x, y, tile.ch, tile.color);

			else if (m_fov->isExplroed({ x, y }))
			{
				Color color = tile.color;
				color.r /= 5;
				color.g /= 5;
				color.b /= 5;
				m_console.setChar(x, y, tile.ch, color);
			}
		}

	for (const auto& entity : m_entities)
	{
		const Vec2i pos = entity->getPosition();

		if (m_fov->isVisible(pos))
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
