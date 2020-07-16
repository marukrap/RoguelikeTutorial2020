#include "Rng.hpp"

#include <cassert>

namespace
{
	std::mt19937 RandomEngine(std::random_device{}());
}

Rng::Rng(unsigned int seed)
	: m_seed(seed)
	, m_engine(seed)
{
}

unsigned int Rng::getSeed() const
{
	return m_seed;
}

void Rng::setSeed(unsigned int seed)
{
	m_seed = seed;
	m_engine.seed(seed);
}

int Rng::getInt(int exclusiveMax)
{
	assert(exclusiveMax > 0);
	return std::uniform_int_distribution<>(0, exclusiveMax - 1)(m_engine);
}

int Rng::getInt(int min, int inclusiveMax)
{
	assert(min <= inclusiveMax);
	return min + std::uniform_int_distribution<>(0, inclusiveMax - min)(m_engine);
}

int randomInt(int exclusiveMax)
{
	assert(exclusiveMax > 0);
	return std::uniform_int_distribution<>(0, exclusiveMax - 1)(RandomEngine);
}

int randomInt(int min, int inclusiveMax)
{
	assert(min <= inclusiveMax);
	return min + std::uniform_int_distribution<>(0, inclusiveMax - min)(RandomEngine);
}
