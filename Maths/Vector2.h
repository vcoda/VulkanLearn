#pragma once

template<typename T>
class Vector2
{
public:
	Vector2() : x(0), y(0) {}
	Vector2(T s) : x(s), y(s) {}
	Vector2(T s1, T s2) : x(s1), y(s2) {}
	Vector2(const Vector2<T>& v) : x(v.x), y(v.y) {}

public:
	const Vector2 operator + (const Vector2<T>& v) const;
	const Vector2 operator - (const Vector2<T>& v) const;
	T operator * (const Vector2<T>& v) const;

	const Vector2 operator + (T s) const;
	const Vector2 operator - (T s) const;
	const Vector2 operator * (T s) const;
	const Vector2 operator / (T s) const;

	Vector2& operator += (T s);
	Vector2& operator -= (T s);
	Vector2& operator *= (T s);
	Vector2& operator /= (T s);

	Vector2& operator += (const Vector2<T>& v);
	Vector2& operator -= (const Vector2<T>& v);
	Vector2& operator *= (const Vector2<T>& v);

	Vector2& operator = (T s);

	bool operator == (const Vector2<T>& v) const;
	bool operator != (const Vector2<T>& v) const;

	T& operator[] (unsigned int index);
	const T& operator[] (unsigned int index) const;

	T SquareLength() const;
	T Length() const;

	Vector2& Normalize();
	const Vector2 Normal() const;

	Vector2& Negativate();
	const Vector2 Negative() const;

public:
	union
	{
		T data[2];
		struct
		{
			T x, y;
		};
		struct
		{
			T r, g;
		};
		struct
		{
			T s, t;
		};
	};
};

#include "Vector2.inl"