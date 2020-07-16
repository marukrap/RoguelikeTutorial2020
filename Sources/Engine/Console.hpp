#pragma once

#include "Atlas.hpp"
#include "Renderer.hpp"
#include "Color.hpp"

#include <SDL2/SDL_ttf.h>

#include <string_view>

// Virtual console
class Console
{
private:
	static constexpr Color White = 0xFFFFFF;
	static constexpr Color Black = 0x000000;
	static constexpr Color Transparent = { 0, 0, 0, 0 };

public:
	Console(TTF_Font& font, int width, int height);

	int getWidth() const;
	int getHeight() const;

	int getTileWidth() const;
	int getTileHeight() const;

	void clear();
	void clear(int left, int top, int width, int height, Color bgColor = Black);
	bool isInBounds(int x, int y) const;

	char getChar(int x, int y) const;
	void setChar(int x, int y, char ch, Color color = White);
	void setString(int x, int y, std::string_view string, Color color = White);

	void setColor(int x, int y, Color color);
	void setColor(int left, int top, int width, int height, Color color);

	void setBgColor(int x, int y, Color color);
	void setBgColor(int left, int top, int width, int height, Color color);

	void drawBox(int left, int top, int width, int height, Color color = White);

	Atlas& getAtlas();
	const std::vector<Sprite>& getSprites();

private:
	void resize(int width, int height);

private:
	static constexpr int Blank = 0x20; // ' '
	static constexpr int Tilde = 0x7E; // '~'

	int m_width = 0;
	int m_height = 0;
	int m_tileWidth = 0;
	int m_tileHeight = 0;
	Atlas m_atlas;
	std::vector<char> m_chars;
	std::vector<Color> m_colors;
	std::vector<Color> m_bgColors;
	std::vector<Sprite> m_sprites;
	bool m_dirty = true;
};
