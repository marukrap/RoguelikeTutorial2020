#include "Atlas.hpp"

#include <SDL2/SDL.h>

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb/stb_rect_pack.h"

Atlas::~Atlas()
{
	if (m_atlas)
		SDL_FreeSurface(m_atlas);

	for (auto& surface : m_sources)
		SDL_FreeSurface(surface);
}

/* For each sprite id, I want to keep its original surface
   and its current location in the texture atlas */
int Atlas::addSurface(SDL_Surface* surface)
{
	// The previous atlas is no longer valid
	if (m_atlas)
	{
		SDL_FreeSurface(m_atlas);
		m_atlas = nullptr;
	}

	const int id = m_sources.size();
	m_sources.push_back(surface);
	SpriteLocation& loc = m_mapping.emplace_back();
	loc.x0 = -0.5f * surface->w;
	loc.y0 = -0.5f * surface->h;
	loc.x1 = +0.5f * surface->w;
	loc.y1 = +0.5f * surface->h;
	// s0,t0,s1,t1 will be filled in during the packing phase

	return id;
}

/* If the surface hasn't been built, or if the set of sprites has
   changed, build the surface, and return it. */
SDL_Surface* Atlas::getSurface()
{
	if (!m_atlas)
	{
		std::vector<stbrp_rect> rects(m_sources.size());
		std::vector<stbrp_node> workingSpace;
		stbrp_context context;
		int atlasSize = 1;

		for (std::size_t i = 0; i < rects.size(); ++i)
		{
			rects[i].id = i;
			rects[i].w = m_sources[i]->w;
			rects[i].h = m_sources[i]->h;

			while (atlasSize < rects[i].w || atlasSize < rects[i].h)
				atlasSize *= 2;
		}

		bool success = false;
		while (!success)
		{
			workingSpace.resize(atlasSize);
			stbrp_init_target(&context, atlasSize, atlasSize, workingSpace.data(), workingSpace.size());
			stbrp_pack_rects(&context, rects.data(), rects.size());

			success = true;
			for (const auto& rect : rects)
			{
				if (!rect.was_packed)
				{
					success = false;
					atlasSize *= 2;
					break;
				}
			}
		}

		m_atlas = SDL_CreateRGBSurface(0, atlasSize, atlasSize, 32,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff
#else
			0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000
#endif
		);

		for (std::size_t i = 0; i < rects.size(); ++i)
		{
			SDL_Surface* surface = m_sources[i];
			SDL_Rect dstRect;
			dstRect.x = rects[i].x;
			dstRect.y = rects[i].y;
			dstRect.w = surface->w;
			dstRect.h = surface->h;

			SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);
			SDL_BlitSurface(surface, nullptr, m_atlas, &dstRect);

			for (std::size_t j = 0; j < m_mapping.size(); ++j)
			{
				m_mapping[i].s0 = static_cast<float>(dstRect.x) / atlasSize;
				m_mapping[i].t0 = static_cast<float>(dstRect.y) / atlasSize;
				m_mapping[i].s1 = static_cast<float>(dstRect.x + dstRect.w) / atlasSize;
				m_mapping[i].t1 = static_cast<float>(dstRect.y + dstRect.h) / atlasSize;
			}
		}
	}

	return m_atlas;
}

const SpriteLocation& Atlas::getLocation(int id) const
{
	return m_mapping[id];
}
