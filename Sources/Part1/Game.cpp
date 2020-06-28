#include "Game.hpp"
#include "Engine/OpenGL.hpp"
#include "Engine/Console.hpp"

Game::Game(SDL_Window& window, Console& console)
	: m_window(window)
	, m_console(console)
	, m_renderer(console.getAtlas())
{
	glClearColor(0.f, 0.f, 0.f, 1.f);

	m_playerX = console.getWidth() / 2;
	m_playerY = console.getHeight() / 2;
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
			switch (event.key.keysym.sym)
			{
			case SDLK_UP:
				--m_playerY;
				break;

			case SDLK_DOWN:
				++m_playerY;
				break;

			case SDLK_LEFT:
				--m_playerX;
				break;

			case SDLK_RIGHT:
				++m_playerX;
				break;
			}
		}
	}
}

void Game::update()
{
	m_console.clear();
	m_console.setChar(m_playerX, m_playerY, '@');
	m_renderer.setSprites(m_console.getSprites());
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	m_renderer.render(&m_window);
	SDL_GL_SwapWindow(&m_window);
}
