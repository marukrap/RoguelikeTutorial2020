#include "Console.hpp"

#include <algorithm> // min, max

Console::Console(TTF_Font& font, int width, int height)
{
	resize(width, height);

	// 95 printable characters
	constexpr int numGlyphs = Tilde - Blank + 1;
	for (int i = 0; i < numGlyphs; ++i)
	{
		const char ch = Blank + i;
		auto surface = TTF_RenderGlyph_Blended(&font, ch, White);

		// Create a white rectangle for rendering colored background rectangles
		if (ch == Blank)
			SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));

		else if (ch == 'A')
		{
			m_tileWidth = surface->w;
			m_tileHeight = surface->h;
		}

		m_atlas.addSurface(surface);
	}

	// Add box-drawing characters
	constexpr int lineThickness = 2;
	const int centerX = (m_tileWidth - lineThickness) / 2;
	const int centerY = (m_tileHeight - lineThickness) / 2;

	for (int i = 0; i < 6; ++i)
	{
		auto surface = SDL_CreateRGBSurface(0, m_tileWidth, m_tileHeight, 32,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff
#else
			0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000
#endif
		);

		std::vector<SDL_Rect> rects;

		switch (i)
		{
		case 0: // '─'
			rects.push_back({ 0, centerY, m_tileWidth, lineThickness });
			break;

		case 1: // '│'
			rects.push_back({ centerX, 0, lineThickness, m_tileHeight });
			break;

		case 2: // '┌'
			rects.push_back({ centerX, centerY, m_tileWidth - centerX, lineThickness });
			rects.push_back({ centerX, centerY, lineThickness, m_tileHeight - centerY });
			break;

		case 3: // '┐'
			rects.push_back({ 0, centerY, centerX, lineThickness });
			rects.push_back({ centerX, centerY, lineThickness, m_tileHeight - centerY });
			break;

		case 4: // '└'
			rects.push_back({ centerX, centerY, m_tileWidth - centerX, lineThickness });
			rects.push_back({ centerX, 0, lineThickness, centerY });
			break;

		case 5: // '┘'
			rects.push_back({ 0, centerY, centerX, lineThickness });
			rects.push_back({ centerX, 0, lineThickness, centerY + lineThickness });
			break;

		// TODO:
			// '├'
			// '┤'
			// '┬'
			// '┴'
			// '┼'
		}

		SDL_FillRects(surface, &rects[0], rects.size(), SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));
		m_atlas.addSurface(surface);
	}
}

int Console::getWidth() const
{
	return m_width;
}

int Console::getHeight() const
{
	return m_height;
}

int Console::getTileWidth() const
{
	return m_tileWidth;
}

int Console::getTileHeight() const
{
	return m_tileHeight;
}

void Console::clear()
{
	std::fill(m_chars.begin(), m_chars.end(), Blank);
	std::fill(m_colors.begin(), m_colors.end(), White);
	std::fill(m_bgColors.begin(), m_bgColors.end(), Transparent);
	m_dirty = true;
}

void Console::clear(int left, int top, int width, int height, Color bgColor)
{
	for (int y = top; y < top + height; ++y)
		for (int x = left; x < left + width; ++x)
			setChar(x, y, Blank);

	setBgColor(left, top, width, height, bgColor);
}

bool Console::isInBounds(int x, int y) const
{
	return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

char Console::getChar(int x, int y) const
{
	return m_chars[x + y * m_width];
}

void Console::setChar(int x, int y, char ch, Color color)
{
	if (isInBounds(x, y)) // && ch >= Blank && ch <= Tilde)
	{
		m_chars[x + y * m_width] = ch;
		m_colors[x + y * m_width] = color;
		m_dirty = true;
	}
}

void Console::setString(int x, int y, std::string_view string, Color color)
{
	int dx = 0;
	int dy = 0;

	for (const char ch : string)
	{
		if (ch == '\t')
		{
			for (int i = 0; i < 4; ++i)
				setChar(x + dx++, y + dy, ' ', color);
		}

		else if (ch == '\n')
		{
			dx = 0;
			++dy;
		}

		else
			setChar(x + dx++, y + dy, ch, color);
	}
}

void Console::setColor(int x, int y, Color color)
{
	if (isInBounds(x, y))
	{
		m_colors[x + y * m_width] = color;
		m_dirty = true;
	}
}

void Console::setColor(int left, int top, int width, int height, Color color)
{
	for (int y = top; y < top + height; ++y)
		for (int x = left; x < left + width; ++x)
			setColor(x, y, color);
}

void Console::setBgColor(int x, int y, Color color)
{
	if (isInBounds(x, y))
	{
		m_bgColors[x + y * m_width] = color;
		m_dirty = true;
	}
}

void Console::setBgColor(int left, int top, int width, int height, Color color)
{
	for (int y = top; y < top + height; ++y)
		for (int x = left; x < left + width; ++x)
			setBgColor(x, y, color);
}

void Console::drawBox(int left, int top, int width, int height, Color color)
{
	constexpr char Horizontal  = 0;
	constexpr char Vertical    = 1;
	constexpr char TopLeft     = 2;
	constexpr char TopRight    = 3;
	constexpr char BottomLeft  = 4;
	constexpr char BottomRight = 5;

	const int right = left + width - 1;
	const int bottom = top + height - 1;

	setChar(left, top, TopLeft, color);
	setChar(right, top, TopRight, color);
	setChar(left, bottom, BottomLeft, color);
	setChar(right, bottom, BottomRight, color);

	for (int x = left + 1; x < right; ++x)
	{
		setChar(x, top, Horizontal, color);
		setChar(x, bottom, Horizontal, color);
	}

	for (int y = top + 1; y < bottom; ++y)
	{
		setChar(left, y, Vertical, color);
		setChar(right, y, Vertical, color);
	}
}

Atlas& Console::getAtlas()
{
	return m_atlas;
}

const std::vector<Sprite>& Console::getSprites()
{
	if (m_dirty)
	{
		m_sprites.clear();

		// Draw a background rectangle
		auto& bg = m_sprites.emplace_back();
		bg.scaleX = static_cast<float>(m_width);
		bg.scaleY = static_cast<float>(m_height);
		bg.r = Black.r / 255.f;
		bg.g = Black.g / 255.f;
		bg.b = Black.b / 255.f;
		bg.a = Black.a / 255.f;

		for (std::size_t i = 0; i < m_chars.size(); ++i)
		{
			const auto [y, x] = std::div(i, m_width);

			if (m_bgColors[i].a > 0)
			{
				auto& s = m_sprites.emplace_back();
				s.x = static_cast<float>(x * m_tileWidth);
				s.y = static_cast<float>(y * m_tileHeight);
				s.r = m_bgColors[i].r / 255.f;
				s.g = m_bgColors[i].g / 255.f;
				s.b = m_bgColors[i].b / 255.f;
				s.a = m_bgColors[i].a / 255.f;
			}

			// Do not draw whitespace
			if (m_colors[i].a > 0 && m_chars[i] != Blank)
			{
				auto& s = m_sprites.emplace_back();
				if (m_chars[i] >= Blank)
					s.id = m_chars[i] - Blank;
				else // Custom characters
					s.id = (Tilde - Blank + 1) + m_chars[i];
				s.x = static_cast<float>(x * m_tileWidth);
				s.y = static_cast<float>(y * m_tileHeight);
				s.r = m_colors[i].r / 255.f;
				s.g = m_colors[i].g / 255.f;
				s.b = m_colors[i].b / 255.f;
				s.a = m_colors[i].a / 255.f;
			}
		}

		m_dirty = false;
	}

	return m_sprites;
}

void Console::resize(int width, int height)
{
	if (width < 0)
		width = 0;
	if (height < 0)
		height = 0;

	m_width = width;
	m_height = height;

	m_chars.resize(width * height, Blank);
	m_colors.resize(width * height, White);
	m_bgColors.resize(width * height, Transparent);

	const std::size_t capacity = width * height * 2 + 1;
	if (m_sprites.capacity() > capacity)
		m_sprites.shrink_to_fit();
	m_sprites.reserve(capacity);

	m_dirty = true;
}
