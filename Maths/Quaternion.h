#pragma once

template<typename T>
class Vector3;

template <typename T>
class Matrix3x3;

template <typename T>
class Quaternion
{
public:
	Quaternion();
	Quaternion(const Quaternion<T>& q);
	Quaternion(const Matrix3x3<T>& m);
	Quaternion(T _w, T _x, T _y, T _z);
	Quaternion(const T* pData);
	Quaternion(T real, const Vector3<T>& imag);
	Quaternion(const Vector3<T>& v, T rotation);

	Matrix3x3<T> Matrix() const;

	bool operator == (const Quaternion<T>& q) const;
	bool operator != (const Quaternion<T>& q) const;
	Quaternion<T>& operator *= (const Quaternion<T>& q);
	Quaternion<T>& operator *= (T s);
	Quaternion<T> operator * (const Quaternion<T>& q) const;
	Quaternion<T> operator * (T s) const;
	Quaternion<T>& operator += (const Quaternion<T>& q);
	Quaternion<T> operator + (const Quaternion<T>& q) const;

	T Mag() const;
	Quaternion<T>& Normalize();
	Quaternion<T> Normal();
	T Dot(const Quaternion<T>& q);
	Quaternion<T> Conjugate() const;

	Vector3<T> Rotate(const Vector3<T>& v);

	// Spherical Lerp
	static Quaternion<T> SLerp(const Quaternion<T>& from, const Quaternion<T>& to, T factor);

	// Normalized Lerp
	static Quaternion<T> NLerp(const Quaternion<T>& from, const Quaternion<T>& to, T factor);

	static T Dot(const Quaternion<T>& q0, const Quaternion<T>& q1);

public:
	union
	{
		struct
		{
			T w, x, y, z;
		};
		struct
		{
			T real;
			Vector3<T> imag;
		};
	};
};

typedef Quaternion<float> Quaternionf;

#include "Quaternion.inl"