#pragma once

#include <random>
#include <vector>
#include <algorithm> // shuffle
#include <cassert>

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

	template <typename T>
	const T& pickOne(const std::vector<T>& vector);

	template <typename T>
	const T& pickOneWeighted(const std::vector<std::pair<T, int>>& vector);

private:
	unsigned int m_seed;
	std::mt19937 m_engine;
};

template <typename T>
void Rng::shuffle(std::vector<T>& vector)
{
	std::shuffle(vector.begin(), vector.end(), m_engine);
}

template <typename T>
const T& Rng::pickOne(const std::vector<T>& vector)
{
	const std::size_t size = vector.size();
	const std::size_t i = std::uniform_int_distribution<std::size_t>(0, size - 1)(m_engine);

	return vector[i];
}

template <typename T>
const T& Rng::pickOneWeighted(const std::vector<std::pair<T, int>>& vector)
{
	int total = 0;

	for (const auto& v : vector)
		total += v.second;

	int r = getInt(total);

	for (const auto& v : vector)
	{
		r -= v.second;

		if (r < 0)
			return v.first;
	}

	assert(false);
	return vector.back().first;
}

int randomInt(int exclusiveMax);
int randomInt(int min, int inclusiveMax);
