#include <cassert>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#include "MathHelpers.h"

namespace dae
{
	Vector4::Vector4(float _x, float _y, float _z, float _w) 
		: x{ _x }, y{ _y }, z{ _z }, w{ _w }
	{
	}

	Vector4::Vector4(const Vector3& v, const float _w) 
		: x{ v.x }, y{ v.y }, z{ v.z }, w{ _w }
	{
	}

	float Vector4::Magnitude() const
	{
		return sqrtf(x * x + y * y + z * z + w * w);
	}

	float Vector4::SqrMagnitude() const
	{
		return x * x + y * y + z * z + w * w;
	}

	float Vector4::Normalize()
	{
		const float m{ Magnitude() };
		if (m > 0)
		{
			const float invM{ 1.f / m };
			x *= invM;
			y *= invM;
			z *= invM;
			w *= invM;
		}

		return m;
	}

	Vector4 Vector4::Normalized() const
	{
		const float invM{ 1.f / Magnitude() };
		return { *this * invM };
	}

	Vector2 Vector4::GetXY() const
	{
		return { x, y };
	}

	Vector3 Vector4::GetXYZ() const
	{
		return { x,y,z };
	}

	float Vector4::Dot(const Vector4& v1, const Vector4& v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
	}

#pragma region Operator Overloads
	Vector4 Vector4::operator*(float scale) const
	{
		return { x * scale, y * scale, z * scale, w * scale };
	}

	Vector4 Vector4::operator+(const Vector4& v) const
	{
		return { x + v.x, y + v.y, z + v.z, w + v.w };
	}

	Vector4 Vector4::operator-(const Vector4& v) const
	{
		return { x - v.x, y - v.y, z - v.z, w - v.w };
	}

	Vector4& Vector4::operator+=(const Vector4& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	float& Vector4::operator[](int index)
	{
		switch (index)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		case 3: return w;
		}

		// cant be right
		assert(false);
		return x;
	}

	float Vector4::operator[](int index) const
	{
		switch (index)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		case 3: return w;
		}

		// cant be right
		assert(false);
		return x;
	}

	bool Vector4::operator==(const Vector4& v) const
	{
		return AreEqual(x, v.x, .000001f) && AreEqual(y, v.y, .000001f) && AreEqual(z, v.z, .000001f) && AreEqual(w, v.w, .000001f);
	}

#pragma endregion
}
