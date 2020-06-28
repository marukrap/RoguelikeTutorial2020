#pragma once

#include <random>

// Random number generator
class Rng
{
public:
	explicit Rng(unsigned int seed = std::random_device()());

	unsigned int getSeed() const;
	void setSeed(unsigned int seed);

	int getInt(int exclusiveMax);          // [0, max)
	int getInt(int min, int inclusiveMax); // [min, max]

private:
	unsigned int m_seed;
	std::mt19937 m_engine;
};
