#pragma once

#include <random>
#include <vector>
#include <algorithm> // shuffle

// Random number generator
class Rng
{
public:
	explicit Rng(unsigned int seed = std::random_device()());

	unsigned int getSeed() const;
	void setSeed(unsigned int seed);

	int getInt(int exclusiveMax);          // [0, max)
	int getInt(int min, int inclusiveMax); // [min, max]

	template <typename T>
	void shuffle(std::vector<T>& vector);

private:
	unsigned int m_seed;
	std::mt19937 m_engine;
};

template <typename T>
void Rng::shuffle(std::vector<T>& vector)
{
	std::shuffle(vector.begin(), vector.end(), m_engine);
}

int randomInt(int exclusiveMax);
int randomInt(int min, int inclusiveMax);
