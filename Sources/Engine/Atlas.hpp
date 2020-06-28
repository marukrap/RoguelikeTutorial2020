// Credit: https://github.com/redblobgames/helloworld-sdl2-opengl-emscripten

#pragma once

#include <vector>

struct SDL_Surface;

struct SpriteLocation
{
	float x0, y0, x1, y1; // Corners in world coordinates
	float s0, t0, s1, t1; // Corners in texture coordinates
};

class Atlas
{
public:
	Atlas() = default;
	~Atlas();

	int addSurface(SDL_Surface* surface);
	SDL_Surface* getSurface();
	const SpriteLocation& getLocation(int id) const;

private:
	SDL_Surface* m_atlas = nullptr;
	std::vector<SDL_Surface*> m_sources;
	std::vector<SpriteLocation> m_mapping;
};
