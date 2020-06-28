#pragma once

#include <cmath> // sqrt

template <typename T>
class Vector2
{
public:
	constexpr Vector2() = default;
	constexpr Vector2(T x, T y);

	constexpr bool operator==(const Vector2& v) const;
	constexpr bool operator!=(const Vector2& v) const;

	constexpr Vector2 operator-() const;

	constexpr Vector2 operator+(const Vector2& v) const;
	constexpr Vector2 operator-(const Vector2& v) const;
	constexpr Vector2 operator*(T v) const;
	constexpr Vector2 operator/(T v) const;

	constexpr Vector2& operator+=(const Vector2& v);
	constexpr Vector2& operator-=(const Vector2& v);
	constexpr Vector2& operator*=(T v);
	constexpr Vector2& operator/=(T v);

	constexpr T length() const;
	constexpr T lengthSquared() const;

public:
	T x = 0;
	T y = 0;
};
template <typename T>
constexpr Vector2<T>::Vector2(T x, T y)
	: x(x)
	, y(y)
{
}

template <typename T>
constexpr bool Vector2<T>::operator==(const Vector2<T>& v) const
{
	return (x == v.x) && (y == v.y);
}

template <typename T>
constexpr bool Vector2<T>::operator!=(const Vector2<T>& v) const
{
	return (x != v.x) || (y != v.y);
}

template<typename T>
constexpr Vector2<T> Vector2<T>::operator-() const
{
	return Vector2<T>(-x, -y);
}

template <typename T>
constexpr Vector2<T> Vector2<T>::operator+(const Vector2& v) const
{
	return Vector2<T>(x + v.x, y + v.y);
}

template <typename T>
constexpr Vector2<T> Vector2<T>::operator-(const Vector2& v) const
{
	return Vector2<T>(x - v.x, y - v.y);
}

template <typename T>
constexpr Vector2<T> Vector2<T>::operator*(T v) const
{
	return Vector2<T>(x * v, y * v);
}

template <typename T>
constexpr Vector2<T> Vector2<T>::operator/(T v) const
{
	return Vector2<T>(x / v, y / v);
}

template <typename T>
constexpr Vector2<T>& Vector2<T>::operator+=(const Vector2& v)
{
	x += v.x;
	y += v.y;

	return *this;
}

template <typename T>
constexpr Vector2<T>& Vector2<T>::operator-=(const Vector2& v)
{
	x -= v.x;
	y -= v.y;

	return *this;
}

template <typename T>
constexpr Vector2<T>& Vector2<T>::operator*=(T v)
{
	x *= v;
	y *= v;

	return *this;
}

template <typename T>
constexpr Vector2<T>& Vector2<T>::operator/=(T v)
{
	x /= v;
	y /= v;

	return *this;
}

template <typename T>
constexpr T Vector2<T>::length() const
{
	return static_cast<T>(std::sqrt(x * x + y * y));
}

template <typename T>
constexpr T Vector2<T>::lengthSquared() const
{
	return x * x + y * y;
}

using Vec2i = Vector2<int>;
using Vec2u = Vector2<unsigned int>;
using Vec2f = Vector2<float>;
using Vec2d = Vector2<double>;
