#ifndef VECTOR2_H
#define VECTOR2_H

namespace dae
{
	struct Int2;

	struct Vector2
	{
		float x;
		float y;

		Vector2() = default;
		Vector2(float _x, float _y);
		Vector2(const Vector2& from, const Vector2& to);

		float Magnitude() const;
		float SqrMagnitude() const;
		float Normalize();
		Vector2 Normalized() const;

		static float Dot(const Vector2& v1, const Vector2& v2);
		static float Cross(const Vector2& v1, const Vector2& v2);

		//Member Operators
		Vector2 operator*(float scale) const;
		Vector2 operator/(float scale) const;
		Vector2 operator+(const Vector2& v) const;
		Vector2 operator-(const Vector2& v) const;
		Vector2 operator-() const;
		Vector2& operator+=(const Vector2& v);
		Vector2& operator-=(const Vector2& v);
		Vector2& operator/=(float scale);
		Vector2& operator*=(float scale);
		float& operator[](const int index);
		float operator[](const int index) const;

		bool operator==(const Vector2& v) const;

		static const Vector2 UnitX;
		static const Vector2 UnitY;
		static const Vector2 Zero;
	};

	//Global Operators
	inline Vector2 operator*(float scale, const Vector2& v)
	{
		return { v.x * scale, v.y * scale };
	}
}

#endif // !VECTOR2_H
