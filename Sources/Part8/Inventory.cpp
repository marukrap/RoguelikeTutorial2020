#include "Inventory.hpp"
#include "World.hpp"
#include "Engine/Console.hpp"

#include <algorithm> // find_if

Inventory::Inventory(std::size_t maxSize)
	: m_maxSize(maxSize)
{
}

std::size_t Inventory::getMaxSize() const
{
	return m_maxSize;
}

std::size_t Inventory::getNumItems() const
{
	return m_items.size();
}

Item* Inventory::at(std::size_t i)
{
	if (i < m_items.size())
		return m_items[i].get();

	return nullptr;
}

bool Inventory::isEmpty() const
{
	return m_items.empty();
}

bool Inventory::isFull() const
{
	return m_items.size() >= m_maxSize;
}

void Inventory::pack(Item::Ptr item)
{
	m_items.push_back(std::move(item));
}

Item::Ptr Inventory::unpack(Item& item)
{
	const auto found = std::find_if(m_items.begin(), m_items.end(),
		[&] (const auto& i) { return i.get() == &item; });

	auto result = std::move(*found);
	m_items.erase(found);

	return result;
}

Item::Ptr Inventory::unpack(std::size_t i)
{
	auto result = std::move(m_items[i]);
	m_items.erase(m_items.begin() + i);

	return result;
}

int Inventory::getGold() const
{
	return m_gold;
}

InventoryMenu::InventoryMenu(Inventory& inventory)
	: m_inventory(inventory)
{
}

bool InventoryMenu::hasSelection() const
{
	return m_selectedItem >= 0;
}

void InventoryMenu::select(std::size_t i)
{
	if (i < m_inventory.getNumItems())
	{
		m_selectedItem = i;

		if (m_selectedButton < 0)
			m_selectedButton = 0;
	}
}

void InventoryMenu::selectPrevious()
{
	if (!m_inventory.isEmpty())
	{
		if (m_selectedItem < 0)
			m_selectedItem = 0;

		select((m_selectedItem + m_inventory.getNumItems() - 1) % m_inventory.getNumItems());
	}
}

void InventoryMenu::selectNext()
{
	if (!m_inventory.isEmpty())
		select((m_selectedItem + 1) % m_inventory.getNumItems());
}

bool InventoryMenu::hasButtonSelection() const
{
	return m_selectedButton >= 0;
}

void InventoryMenu::selectButton(std::size_t i)
{
	if (i < NumButtons)
		m_selectedButton = i;
}

void InventoryMenu::selectPreviousButton()
{
	if (hasSelection())
		m_selectedButton = (m_selectedButton + NumButtons - 1) % NumButtons;
}

void InventoryMenu::selectNextButton()
{
	if (hasSelection())
		m_selectedButton = (m_selectedButton + 1) % NumButtons;
}

void InventoryMenu::activateButton(World& world)
{
	if (!hasButtonSelection())
		return;

	if (m_selectedButton == Use)
	{
		Item* item = m_inventory.at(m_selectedItem);
		world.useItem(*item);

		if (item->getCount() == 0)
			m_inventory.unpack(m_selectedItem);
	}

	else if (m_selectedButton == Drop)
	{
		Item::Ptr item = m_inventory.unpack(m_selectedItem);
		world.dropItem(std::move(item));
	}

	else if (m_selectedButton == Throw)
	{
		// TODO: ranged attack
		world.throwItem(nullptr);
	}

	world.closeInventory();
}

void InventoryMenu::draw(Console& console)
{
	std::string header = " Inventory $ ";
	header += std::to_string(m_inventory.getGold());
	header += ' ';

	std::string footer;
	footer += " (";
	footer += std::to_string(m_inventory.getNumItems());
	footer += '/';
	footer += std::to_string(m_inventory.getMaxSize());
	footer += ") ";

	std::size_t width = header.size() + 2;
	std::size_t height = m_inventory.getNumItems();

	std::vector<std::string> strings;

	for (std::size_t i = 0; i < m_inventory.getNumItems(); ++i)
	{
		std::string& str = strings.emplace_back();
		str += static_cast<char>('a' + i);
		str += ") ";
		str += m_inventory.at(i)->getChar();
		str += ' ';
		const int j = str.size();
		str += m_inventory.at(i)->getAName();
		str[j] = std::toupper(str[j]);
		width = std::max(width, str.size());
	}

	const int x = 18; // (console.getWidth() - width) / 2;
	const int y = 2; // (console.getHeight() - height) / 2;

	console.clear(x - 2, y - 1, width + 4, height + 2);
	console.drawBox(x - 2, y - 1, width + 4, height + 2);

	for (std::size_t i = 0; i < strings.size(); ++i)
	{
		console.setString(x, y + i, strings[i]);
		console.setColor(x + 3, y + i, m_inventory.at(i)->getColor());
	}

	if (hasSelection())
	{
		console.setBgColor(x - 1, y + m_selectedItem, width + 2, 1, 0x3B1725);

		// Draw item description window
		std::string itemName(m_inventory.at(m_selectedItem)->getAName());
		itemName[0] = std::toupper(itemName[0]);
		const std::string_view itemDescription = m_inventory.at(m_selectedItem)->getDescription();
		const std::vector<std::string_view> itemButtons = { "apply", "drop", "throw" };

		constexpr int itemButtonWidth = 9;

		const int itemWindowWidth = (itemButtonWidth + 1) * itemButtons.size() - 1;
		const int itemWindowHeight = 5;

		const int itemX = x + width + 3;
		const int itemY = std::min(y + m_selectedItem, console.getHeight() - itemWindowHeight - 2);

		console.clear(itemX - 1, itemY - 1, itemWindowWidth + 2, itemWindowHeight + 2);
		console.drawBox(itemX - 1, itemY - 1, itemWindowWidth + 2, itemWindowHeight + 2);

		console.setString(itemX + (itemWindowWidth - itemName.size()) / 2, itemY, itemName);
		console.setBgColor(itemX, itemY, itemWindowWidth, 1, 0x333941);

		console.setString(itemX + 1, itemY + 2, itemDescription);

		int currentX = itemX;

		for (const auto& button : itemButtons)
		{
			console.setString(currentX + (itemButtonWidth - button.size()) / 2, itemY + 4, button);
			console.setColor(currentX + (itemButtonWidth - button.size()) / 2, itemY + 4, 0xFFD541);
			currentX += itemButtonWidth + 1;
		}

		console.setBgColor(itemX + (itemButtonWidth + 1) * m_selectedButton, itemY + 4, itemButtonWidth, 1, 0x143464);
	}

	console.setString(x + (width - header.size()) / 2, y - 1, header);
	console.setColor(x + (width - header.size()) / 2 + 11, y - 1, 0xFFD541);

	console.setString(x + (width - footer.size()) / 2, y + height, footer, 0x333941);
}
