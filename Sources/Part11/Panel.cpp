#include "Panel.hpp"
#include "Entity/Player.hpp"
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
	constexpr Color blue = 0x285CC4;
	constexpr Color purple = 0x793A80;

	int hpBarWidth = 0;
	std::string hpText;

	// TODO: Keep the player actor instance
	if (m_player && !m_player->isDestroyed())
	{
		const int hp = m_player->getHp();
		const int maxHp = m_player->getMaxHp();

		hpBarWidth = std::max(1, barWidth * hp / maxHp);
		hpText = "HP " + std::to_string(hp) + "/" + std::to_string(maxHp);

		const int textPosX = m_position.x + (barWidth - hpText.size()) / 2;

		console.setBgColor(m_position.x, m_position.y + 1, hpBarWidth, 1, red);
		console.setString(textPosX, m_position.y + 1, hpText);

		std::string statusText;

		if (m_player->hasStatusEffect(StatusEffect::Confused))
			statusText = "Confused";

		if (!statusText.empty())
			console.setString(m_position.x + (barWidth - statusText.size()) / 2, m_position.y, statusText, purple);

		Player* player = static_cast<Player*>(m_player);

		const std::string xpText = "XP " + std::to_string(player->getXp());
		const int xpBarWidth = barWidth * player->getXp() / player->getXpToNextLevel();

		console.setBgColor(m_position.x, m_position.y + 2, xpBarWidth, 1, blue);
		console.setString(m_position.x + (barWidth - xpText.size()) / 2, m_position.y + 2, xpText);

		const std::string levelText = "Level " + std::to_string(player->getLevel());
		const std::string statsText = "Atk " + std::to_string(player->getAttack()) + "  Def " + std::to_string(player->getDefense());

		console.setString(m_position.x + (barWidth - levelText.size()) / 2, m_position.y + 3, levelText);
		console.setString(m_position.x + (barWidth - statsText.size()) / 2, m_position.y + 4, statsText);
	}

	else
	{
		const std::string_view statusText = "Dead";

		console.setString(m_position.x + (barWidth - statusText.size()) / 2, m_position.y, statusText, red);
	}

	for (std::size_t i = 0; i < m_messages.size(); ++i)
		console.setString(m_position.x + barWidth + 1, m_position.y + i, m_messages[i]);
}

void Panel::save(std::ostream& os)
{
	const std::size_t size = m_messages.size();
	serialize(os, size);

	for (std::size_t i = 0; i < size; ++i)
		serialize(os, m_messages[i]);
}

void Panel::load(std::istream& is)
{
	std::size_t size;
	deserialize(is, size);

	m_messages.resize(size);
	for (std::size_t i = 0; i < size; ++i)
		deserialize(is, m_messages[i]);
}
