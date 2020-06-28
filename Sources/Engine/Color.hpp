#pragma once

#include <SDL2/SDL.h>

class Color
{
public:
	constexpr Color() = default;
	constexpr Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 0xFF);
	constexpr Color(unsigned int hex, Uint8 a = 0xFF);

	constexpr Color(const SDL_Color& color);
	constexpr Color& operator=(const SDL_Color& color);
	constexpr operator SDL_Color() const;

	constexpr unsigned int toHexRGB() const;
	constexpr unsigned int toHexRGBA() const;

public:
	Uint8 r = 0xFF;
	Uint8 g = 0xFF;
	Uint8 b = 0xFF;
	Uint8 a = 0xFF;
};

constexpr Color::Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
	: r(r)
	, g(g)
	, b(b)
	, a(a)
{
}

constexpr Color::Color(unsigned int hex, Uint8 a)
	: r((hex >> 16) & 0xFF)
	, g((hex >> 8) & 0xFF)
	, b(hex & 0xFF)
	, a(a)
{
}

constexpr Color::Color(const SDL_Color& color)
	: r(color.r)
	, g(color.g)
	, b(color.b)
	, a(color.a)
{
}

constexpr Color& Color::operator=(const SDL_Color& color)
{
	r = color.r;
	g = color.g;
	b = color.b;
	a = color.a;

	return *this;
}

constexpr Color::operator SDL_Color() const
{
	return { r, g, b, a };
}

constexpr unsigned int Color::toHexRGB() const
{
	return (r << 16) | (g << 8) | b;
}

constexpr unsigned int Color::toHexRGBA() const
{
	return (r << 24) | (g << 16) | (b << 8) | a;
}
