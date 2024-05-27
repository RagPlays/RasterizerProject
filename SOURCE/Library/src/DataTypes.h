
#ifndef DATATYPES_H
#define DATATYPES_H

#include "Maths.h"
#include <vector>

namespace dae
{
	struct Vector3;
	struct ColorRGB;

	struct Vertex
	{
		Vertex()
			: position{}, color{ colors::Magenta }, uv{}
		{
		}

		Vertex(const Vector3& _position, const ColorRGB& _color, const Vector2& _uv)
			: position{ _position }, color{ _color }, uv{ _uv }
		{
		}

		Vertex(const Vector3& _position)
			: position{ _position }, color{ colors::Magenta }, uv{}
		{
		}

		Vertex(const Vector3& _position, const Vector2& _uv)
			: position{ _position }, color{ colors::Magenta }, uv{ _uv }
		{
		}

		Vector3 position;
		ColorRGB color;
		Vector2 uv;
		Vector3 normal{};
		Vector3 tangent{};
		Vector3 viewDirection{};
	};

	struct Vertex_Out
	{
		Vertex_Out() = default;
		Vertex_Out(const Vector4& _position, const ColorRGB& _color, const Vector2& _uv, const Vector3& _normal, const Vector3& _tangent, const Vector3& _viewDir)
			: position{ _position },
			color{ _color },
			uv{ _uv },
			normal{ _normal },
			tangent{ _tangent },
			viewDirection{ _viewDir }
		{
		}

		Vector4 position{};
		ColorRGB color{ colors::White };
		Vector2 uv{};
		Vector3 normal{};
		Vector3 tangent{};
		Vector3 viewDirection{};
	};

	enum class PrimitiveTopology
	{
		TriangleList,
		TriangleStrip
	};

	struct Mesh
	{
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};
		PrimitiveTopology primitiveTopology{ PrimitiveTopology::TriangleStrip };

		std::vector<Vertex_Out> vertices_out{};
		Matrix worldMatrix{};
	};
}

#endif // !DATATYPES_H