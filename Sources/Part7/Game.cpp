#include "Game.hpp"
#include "Engine/OpenGL.hpp"
#include "Engine/Console.hpp"

Game::Game(SDL_Window& window, Console& console)
	: m_window(window)
	, m_console(console)
	, m_renderer(console.getAtlas())
	, m_world(console.getWidth(), console.getHeight())
{
	glClearColor(0.f, 0.f, 0.f, 1.f);
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
				m_world.waitPlayer();
				break;
			}

			if (dx != 0 || dy != 0)
				m_world.movePlayer(dx, dy);
		}
	}
}

void Game::update()
{
	m_world.update(m_console);
	m_renderer.setSprites(m_console.getSprites());
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	m_renderer.render(&m_window);
	SDL_GL_SwapWindow(&m_window);
}
