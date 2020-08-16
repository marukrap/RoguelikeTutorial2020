#pragma once

#include "Entity.hpp"

#include <memory>

struct ItemData;
class Actor;

class Item : public Entity
{
public:
	using Ptr = std::unique_ptr<Item>;

public:
	explicit Item(const std::string& name);

	std::string_view getName() const override;
	std::string getAName() const override;
	std::string_view getDescription() const;

	int getCount() const;
	void setCount(int count);

	// Apply functions
	void apply(Actor& actor);
	void heal(Actor& actor);
	void confuse(Actor& actor);

	void throwAt(Actor& actor);

	void save(std::ostream& os) override;
	void load(std::istream& is) override;

private:
	const std::string m_name;
	const ItemData& m_data;

	int m_count = 1;
};
