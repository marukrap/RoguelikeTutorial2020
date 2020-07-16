#include "Panel.hpp"
#include "Actor.hpp"
#include "Engine/Console.hpp"

#include <algorithm> // min
#include <cctype>

Panel::Panel(int x, int y, int width, int height)
	: m_position(x, y)
	, m_size(width, height)
{
}

void Panel::setPlayer(Actor* player)
{
	m_player = player;
}

void Panel::addMessage(std::string&& message)
{
	if (!message.empty())
		message[0] = std::toupper(message[0]);

	m_messages.push_back(std::move(message));

	if (static_cast<int>(m_messages.size()) > m_size.y)
		m_messages.pop_front();
}

void Panel::draw(Console& console)
{
	constexpr int barWidth = 16;
	constexpr Color red = 0xB4202A;

	int hpBarWidth = 0;
	std::string hpText;
	Color hpColor = 0xFFFFFF;

	if (m_player && !m_player->isDestroyed())
	{
		const int hp = m_player->getHp();
		const int maxHp = m_player->getMaxHp();

		hpBarWidth = std::max(1, barWidth * hp / maxHp);
		hpText = "HP " + std::to_string(hp) + "/" + std::to_string(maxHp);
	}

	else
	{
		hpText = "Dead";
		hpColor = red;
	}

	const int textPosX = m_position.x + (barWidth - hpText.size()) / 2;

	console.setBgColor(m_position.x, m_position.y, hpBarWidth, 1, red);
	console.setString(textPosX, m_position.y, hpText, hpColor);

	for (std::size_t i = 0; i < m_messages.size(); ++i)
		console.setString(m_position.x + barWidth + 1, m_position.y + i, m_messages[i]);
}
