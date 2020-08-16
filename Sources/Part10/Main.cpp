#include "Game.hpp"
#include "Engine/OpenGL.hpp"
#include "Engine/Console.hpp"

#include <iostream>
#include <memory>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#include <sstream>

void onerror(void* arg)
{
}

void onstore(void* arg)
{
}

const char* beforeunload_callback(int eventType, const void* reserved, void* userData)
{
	Game* game = static_cast<Game*>(userData);
	std::ostringstream oss;
	game->save(oss);
	std::string str = oss.str();
	if (!str.empty())
		emscripten_idb_async_store("Part10", "Savefile", &str[0], str.size(), 0, onstore, onerror);

	return "";
}
#endif

void main_loop(void* arg)
{
	Game* game = static_cast<Game*>(arg);
	game->tick();
}

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_VIDEO);

	TTF_Init();
	TTF_Font* font = TTF_OpenFont("Fonts/RecMono-Casual.ttf", 20);

	constexpr int consoleWidth = 80;
	constexpr int consoleHeight = 30;
	auto console = std::make_unique<Console>(*font, consoleWidth, consoleHeight);

	TTF_CloseFont(font);
	TTF_Quit();

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	const int windowWidth = consoleWidth * console->getTileWidth();
	const int windowHeight = consoleHeight * console->getTileHeight();
	const Uint32 windowFlags = SDL_WINDOW_OPENGL;
	SDL_Window* window = SDL_CreateWindow("Roguelike Tutorial 2020", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, windowFlags);
	SDL_GLContext glContext = SDL_GL_CreateContext(window);

	SDL_GL_SetSwapInterval(1); // Enable vsync

#ifndef __EMSCRIPTEN__
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize OpenGL Loader.\n";
		return 1;
	}
#endif

	Game game(*window, *console);

#ifdef __EMSCRIPTEN__
	emscripten_set_beforeunload_callback(&game, beforeunload_callback);

	constexpr int fps = 0; // 0 means to use the browser's requestAnimationFrame
	constexpr int simulate_infinite_loop = 1;
	emscripten_set_main_loop_arg(main_loop, &game, fps, simulate_infinite_loop);
#else
	while (game.isRunning())
		game.tick();
#endif

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
