#include <cassert>

#include "Vector3.h"
#include "Vector4.h"

#include "MathHelpers.h"

#include "Matrix.h"

namespace dae {
	Matrix::Matrix(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis, const Vector3& t) :
		Matrix({ xAxis, 0.f }, { yAxis, 0.f }, { zAxis, 0.f }, { t, 1.f })
	{
	}

	Matrix::Matrix(const Vector4& xAxis, const Vector4& yAxis, const Vector4& zAxis, const Vector4& t)
	{
		data[0] = xAxis;
		data[1] = yAxis;
		data[2] = zAxis;
		data[3] = t;
	}

	Matrix::Matrix(const Matrix& m)
	{
		data[0] = m[0];
		data[1] = m[1];
		data[2] = m[2];
		data[3] = m[3];
	}

	Vector3 Matrix::TransformVector(const Vector3& v) const
	{
		return TransformVector(v.x, v.y, v.z);
	}

	Vector3 Matrix::TransformVector(float x, float y, float z) const
	{
		return Vector3
		{
			data[0].x * x + data[1].x * y + data[2].x * z,
			data[0].y * x + data[1].y * y + data[2].y * z,
			data[0].z * x + data[1].z * y + data[2].z * z
		};
	}

	Vector3 Matrix::TransformPoint(const Vector3& p) const
	{
		return TransformPoint(p.x, p.y, p.z);
	}

	Vector3 Matrix::TransformPoint(float x, float y, float z) const
	{
		return Vector3
		{
			data[0].x * x + data[1].x * y + data[2].x * z + data[3].x,
			data[0].y * x + data[1].y * y + data[2].y * z + data[3].y,
			data[0].z * x + data[1].z * y + data[2].z * z + data[3].z,
		};
	}

	Vector4 Matrix::TransformPoint(const Vector4& p) const
	{
		return TransformPoint(p.x, p.y, p.z, p.w);
	}

	Vector4 Matrix::TransformPoint(float x, float y, float z, float w) const
	{
		return Vector4
		{
			data[0].x * x + data[1].x * y + data[2].x * z + data[3].x,
			data[0].y * x + data[1].y * y + data[2].y * z + data[3].y,
			data[0].z * x + data[1].z * y + data[2].z * z + data[3].z,
			data[0].w * x + data[1].w * y + data[2].w * z + data[3].w
		};
	}

	const Matrix& Matrix::Transpose()
	{
		const int nrOfVectorsInMatrix{ 4 };
		for (int r{ 0 }; r < nrOfVectorsInMatrix; ++r)
		{
			for (int c{ r + 1 }; c < nrOfVectorsInMatrix; ++c)
			{
				std::swap(data[r][c], data[c][r]);
			}
		}
		return *this;
	}

	const Matrix& Matrix::Inverse()
	{
		//Optimized Inverse as explained in FGED1 - used widely in other libraries too.
		const Vector3& a{ data[0] };
		const Vector3& b{ data[1] };
		const Vector3& c{ data[2] };
		const Vector3& d{ data[3] };

		const float x{ data[0][3] };
		const float y{ data[1][3] };
		const float z{ data[2][3] };
		const float w{ data[3][3] };

		Vector3 s{ Vector3::Cross(a, b) };
		Vector3 t{ Vector3::Cross(c, d) };
		Vector3 u{ a * y - b * x };
		Vector3 v{ c * w - d * z };

		const float det{ Vector3::Dot(s, v) + Vector3::Dot(t, u) };
		assert((!AreEqual(det, 0.f)) && "ERROR: determinant is 0, there is no INVERSE!");

		const float invDet{ 1.f / det };
		s *= invDet; 
		t *= invDet; 
		u *= invDet; 
		v *= invDet;

		const Vector3 r0{ Vector3::Cross(b, v) + t * y };
		const Vector3 r1{ Vector3::Cross(v, a) - t * x };
		const Vector3 r2{ Vector3::Cross(d, u) + s * w };
		const Vector3 r3{ Vector3::Cross(u, c) - s * z };

		data[0] = { r0.x, r1.x, r2.x, 0.f };
		data[1] = { r0.y, r1.y, r2.y, 0.f };
		data[2] = { r0.z, r1.z, r2.z, 0.f };
		data[3] = { { -Vector3::Dot(b, t) }, {Vector3::Dot(a, t) }, {-Vector3::Dot(d, s) }, {Vector3::Dot(c, s) } };

		return *this;
	}

	Matrix Matrix::Transpose(Matrix m)
	{
		return m.Transpose();
	}

	Matrix Matrix::Inverse(Matrix m)
	{
		return m.Inverse();
	}

	Vector3 Matrix::GetAxisX() const
	{
		return data[0];
	}

	Vector3 Matrix::GetAxisY() const
	{
		return data[1];
	}

	Vector3 Matrix::GetAxisZ() const
	{
		return data[2];
	}

	Vector3 Matrix::GetTranslation() const
	{
		return data[3];
	}

	Matrix Matrix::CreateTranslation(float x, float y, float z)
	{
		return CreateTranslation({ x, y, z });
	}

	Matrix Matrix::CreateTranslation(const Vector3& t)
	{
		return { Vector3::UnitX, Vector3::UnitY, Vector3::UnitZ, t };
	}

	Matrix Matrix::CreateRotationX(float pitch)
	{
		const float cospitch{ cosf(pitch) };
		const float sinpitch{ sinf(pitch) };

		return 
		{ 
			{1.f, 0.f, 0.f, 0.f}, 
			{0.f, cospitch, sinpitch, 0.f}, 
			{0.f, -sinpitch, cospitch, 0.f}, 
			{0.f, 0.f, 0.f, 1.f} 
		};
	}

	Matrix Matrix::CreateRotationY(float yaw)
	{
		const float cosyaw{ cosf(yaw) };
		const float sinyaw{ sinf(yaw) };

		return 
		{ 
			{cosyaw, 0.f, -sinyaw, 0.f}, 
			{0.f, 1.f, 0.f, 0.f}, 
			{sinyaw, 0.f, cosyaw, 0.f }, 
			{0.f, 0.f, 0.f, 1.f} 
		};
	}

	Matrix Matrix::CreateRotationZ(float roll)
	{
		const float cosroll{ cosf(roll) };
		const float sinroll{ sinf(roll) };

		return 
		{ 
			{cosroll, sinroll, 0.f , 0.f}, 
			{-sinroll, cosroll, 0.f, 0.f},
			{0.f, 0.f, 1.f, 0.f}, 
			{0.f, 0.f, 0.f, 1.f} 
		};
	}

	Matrix Matrix::CreateRotation(float pitch, float yaw, float roll)
	{
		return CreateRotation({ pitch, yaw, roll });
	}

	Matrix Matrix::CreateRotation(const Vector3& r)
	{
		return CreateRotationX(r[0]) * CreateRotationY(r[1]) * CreateRotationZ(r[2]);
	}

	Matrix Matrix::CreateScale(float sx, float sy, float sz)
	{
		return { {sx, 0, 0}, {0, sy, 0}, {0, 0, sz}, Vector3::Zero };
	}

	Matrix Matrix::CreateScale(const Vector3& s)
	{
		return CreateScale(s[0], s[1], s[2]);
	}

	Matrix Matrix::CreateLookAtLH(const Vector3& origin, const Vector3& forward, const Vector3& up)
	{
		/*zaxis = normal(At - Eye)
		xaxis = normal(cross(Up, zaxis))
		yaxis = cross(zaxis, xaxis)

		xaxis.x           yaxis.x           zaxis.x          0
		xaxis.y           yaxis.y           zaxis.y          0
		xaxis.z           yaxis.z           zaxis.z          0
		- dot(xaxis, eye) - dot(yaxis, eye) - dot(zaxis, eye)  1*/

		const Vector3 zAxis{ (forward - origin).Normalized() };
		const Vector3 xAxis{ Vector3::Cross(up, zAxis).Normalized() };
		const Vector3 yAxis{ Vector3::Cross(zAxis, xAxis) };

		return
		{
			{xAxis.x, yAxis.x, zAxis.x, 0.f},
			{xAxis.y, yAxis.y, zAxis.y, 0.f},
			{xAxis.z, yAxis.z, zAxis.z, 0.f},
			{-Vector3::Dot(xAxis, origin), -Vector3::Dot(yAxis, origin), -Vector3::Dot(zAxis, origin), 1.f}
		};
	}



	Matrix Matrix::CreatePerspectiveFovLH(float fov, float aspectRatio, float near, float far)
	{
		// far -> max view distance
		// near -> min view distance
		
		// worldViewProjectionMatrix //
		// A = far / (far - near)
		// B = -(far * near) / (far - near)

		// (1 / (AR*FOV))		0		0		0
		//		0		(1 / FOV)	0		0
		//		0			0		A		1
		//		0			0		B		0

		const float divFOV{ 1.f / fov };
		const float divAspectFOV{ 1.f / aspectRatio * divFOV };
		const float A{ far / (far - near) };
		const float B{ -(far * near) / (far - near) };

		return 
		{
			{	divAspectFOV	, 0.f		, 0.f	, 0.f },
			{	0.f				, divFOV	, 0.f	, 0.f },
			{	0.f				, 0.f		, A		, 1.f },
			{	0.f				, 0.f		, B		, 0.f }
		};
	}

#pragma region Operator Overloads
	Vector4& Matrix::operator[](int index)
	{
		assert(index <= 3 && index >= 0);

		return data[index];
	}

	Vector4 Matrix::operator[](int index) const
	{
		assert(index <= 3 && index >= 0);

		return data[index];
	}

	Matrix Matrix::operator*(const Matrix& m) const
	{
		Matrix result{};
		Matrix m_transposed{ Transpose(m) };

		for (int r{ 0 }; r < 4; ++r)
		{
			for (int c{ 0 }; c < 4; ++c)
			{
				result[r][c] = Vector4::Dot(data[r], m_transposed[c]);
			}
		}

		return result;
	}

	const Matrix& Matrix::operator*=(const Matrix& m)
	{
		Matrix copy{ *this };
		Matrix m_transposed{ Transpose(m) };

		for (int r{ 0 }; r < 4; ++r)
		{
			for (int c{ 0 }; c < 4; ++c)
			{
				data[r][c] = Vector4::Dot(copy[r], m_transposed[c]);
			}
		}

		return *this;
	}

	bool Matrix::operator==(const Matrix& m) const
	{
		return data[0] == m.data[0]
		    && data[1] == m.data[1]
			&& data[2] == m.data[2]
			&& data[3] == m.data[3];
	}

#pragma endregion
}