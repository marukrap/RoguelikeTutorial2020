// Credit: https://github.com/redblobgames/helloworld-sdl2-opengl-emscripten

#pragma once

#include "Atlas.hpp"

#include <memory>

class Atlas;
struct SDL_Window;
struct RendererImpl;

struct Sprite
{
	int id = 0;
	float x = 0.f;
	float y = 0.f;
	float scaleX = 1.f;
	float scaleY = 1.f;
	float r = 1.f;
	float g = 1.f;
	float b = 1.f;
	float a = 1.f;
};

class Renderer
{
public:
	explicit Renderer(Atlas& atlas);
	~Renderer();

	void clearSprites();
	void setSprites(const std::vector<Sprite>& sprites);

	void render(SDL_Window* window);

private:
	std::unique_ptr<RendererImpl> self;
};
