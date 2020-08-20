#pragma once

#include "Entity.hpp"

#include <functional>
#include <memory>

struct ItemData;
class Actor;

class Item : public Entity
{
public:
	using Ptr = std::unique_ptr<Item>;

public:
	Item(const std::string& name, const ItemData& data);

	std::string_view getName() const override;
	std::string getAName() const override;
	std::string_view getDescription() const;

	int getCount() const;
	void setCount(int count);

	// Apply functions
	void apply(Actor& actor);
	void heal(Actor& actor);
	void confuse(Actor& actor);

	virtual void equip(Actor& actor);

	void throwAt(Actor& actor);

	void save(std::ostream& os) override;
	void load(std::istream& is) override;

	static Item::Ptr createItem(const std::string& name);

private:
	const std::string m_name;
	const ItemData& m_data;

	int m_count = 1;
};

struct ItemData
{
	char ch;
	Color color;
	std::string description;
	std::function<void(Item&, Actor&)> apply;
};
