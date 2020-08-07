#pragma once

#include <fstream>
#include <vector>
#include <string>

class Serializable
{
public:
	Serializable() = default;
	virtual ~Serializable() = default;

	Serializable(const Serializable&) = delete;
	Serializable& operator=(const Serializable&) = delete;

	virtual void save(std::ostream& os) = 0;
	virtual void load(std::istream& is) = 0;

protected:
	template <typename T>
	static void serialize(std::ostream& os, const T& data);
	template <typename T>
	static void serialize(std::ostream& os, const std::vector<T>& data);
	template <typename T>
	static void deserialize(std::istream& is, T& data);
	template <typename T>
	static void deserialize(std::istream& is, std::vector<T>& data);
};

template <typename T>
void Serializable::serialize(std::ostream& os, const T& data)
{
	os.write(reinterpret_cast<const char*>(&data), sizeof(data));
}

template <>
inline void Serializable::serialize(std::ostream& os, const std::string& data)
{
	const std::size_t size = data.size();
	serialize(os, size);

	for (std::size_t i = 0; i < size; ++i)
		serialize(os, data[i]);
}

template <>
inline void Serializable::serialize(std::ostream& os, const std::wstring& data)
{
	const std::size_t size = data.size();
	serialize(os, size);

	for (std::size_t i = 0; i < size; ++i)
		serialize(os, data[i]);
}

template <typename T>
void Serializable::serialize(std::ostream& os, const std::vector<T>& data)
{
	const std::size_t size = data.size();
	serialize(os, size);

	for (std::size_t i = 0; i < size; ++i)
		serialize(os, data[i]);
}

template <>
inline void Serializable::serialize(std::ostream& os, const std::vector<bool>& data)
{
	const std::size_t size = data.size();
	serialize(os, size);

	for (std::size_t i = 0; i < size; ++i)
	{
		const bool value = data[i];
		serialize(os, value);
	}
}

template <typename T>
void Serializable::deserialize(std::istream& is, T& data)
{
	is.read(reinterpret_cast<char*>(&data), sizeof(data));
}

template <>
inline void Serializable::deserialize(std::istream& is, std::string& data)
{
	std::size_t size = 0;
	deserialize(is, size);

	data.resize(size);
	for (std::size_t i = 0; i < size; ++i)
		deserialize(is, data[i]);
}

template <>
inline void Serializable::deserialize(std::istream& is, std::wstring& data)
{
	std::size_t size = 0;
	deserialize(is, size);

	data.resize(size);
	for (std::size_t i = 0; i < size; ++i)
		deserialize(is, data[i]);
}

template <typename T>
void Serializable::deserialize(std::istream& is, std::vector<T>& data)
{
	std::size_t size = 0;
	deserialize(is, size);

	data.resize(size);
	for (std::size_t i = 0; i < size; ++i)
		deserialize(is, data[i]);
}

template <>
inline void Serializable::deserialize(std::istream& is, std::vector<bool>& data)
{
	std::size_t size = 0;
	deserialize(is, size);

	data.resize(size);
	for (std::size_t i = 0; i < size; ++i)
	{
		bool value;
		deserialize(is, value);
		data[i] = value;
	}
}
