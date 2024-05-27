
#ifndef COLORRGB_H
#define COLORRGB_H

#include <algorithm>

#include "MathHelpers.h"
#include "SDL.h"

namespace dae
{
	struct ColorRGB
	{
		ColorRGB()
			: r{}, g{}, b{}
		{
		}

		ColorRGB(float _r, float _g, float _b)
			: r{ _r }, g{ _g }, b{ _b }
		{
		}

		ColorRGB(float colorValue)
			: r{ colorValue }, g{ colorValue }, b{ colorValue }
		{
		}

		ColorRGB(const ColorRGB& other)
			: ColorRGB{ other.r, other.g, other.b }
		{
		}

		ColorRGB(const SDL_Color& other)
			: ColorRGB{ static_cast<float>(other.r) / 255.f, static_cast<float>(other.g) / 255.f, static_cast<float>(other.b) / 255.f }
		{
		}

		float r;
		float g;
		float b;

		void MaxToOne()
		{
			const float maxValue{ std::max(r, std::max(g, b)) };
			if (maxValue > 1.f) *this /= maxValue;
		}

		static ColorRGB Lerp(const ColorRGB& c1, const ColorRGB& c2, float factor)
		{
			return { Lerpf(c1.r, c2.r, factor), Lerpf(c1.g, c2.g, factor), Lerpf(c1.b, c2.b, factor) };
		}

#pragma region ColorRGB (Member) Operators
		const ColorRGB& operator+=(const ColorRGB& c)
		{
			r += c.r;
			g += c.g;
			b += c.b;

			return *this;
		}

		ColorRGB operator+(const ColorRGB& c) const
		{
			return { r + c.r, g + c.g, b + c.b };
		}

		const ColorRGB& operator-=(const ColorRGB& c)
		{
			r -= c.r;
			g -= c.g;
			b -= c.b;

			return *this;
		}

		const ColorRGB& operator-=(float s)
		{
			r -= s;
			g -= s;
			b -= s;

			return *this;
		}

		ColorRGB operator-(const ColorRGB& c) const
		{
			return { r - c.r, g - c.g, b - c.b };
		}

		const ColorRGB& operator*=(const ColorRGB& c)
		{
			r *= c.r;
			g *= c.g;
			b *= c.b;

			return *this;
		}

		/*ColorRGB operator*(const ColorRGB& c) const
		{
			return { r * c.r, g * c.g, b * c.b };
		}*/

		const ColorRGB& operator/=(const ColorRGB& c)
		{
			r /= c.r;
			g /= c.g;
			b /= c.b;

			return *this;
		}

		const ColorRGB& operator*=(float s)
		{
			r *= s;
			g *= s;
			b *= s;

			return *this;
		}

		ColorRGB operator*(float s) const
		{
			return { r * s, g * s,b * s };
		}

		const ColorRGB& operator/=(float s)
		{
			const float invScale{ 1.f / s };
			r *= invScale;
			g *= invScale;
			b *= invScale;

			return *this;
		}

		ColorRGB operator/( float s) const
		{
			const float invScale{ 1.f / s };
			return { r * invScale, g * invScale, b * invScale };
		}
#pragma endregion
	};

	//ColorRGB (Global) Operators
	inline ColorRGB operator*(float s, const ColorRGB& c)
	{
		return c * s;
	}

	inline ColorRGB operator*(const ColorRGB& c1, const ColorRGB& c2)
	{
		return
		{
			c1.r * c2.r,
			c1.g * c2.g,
			c1.b * c2.b
		};
	}

	inline ColorRGB operator-(float value, const ColorRGB& c)
	{
		return { value - c.r, value - c.g, value - c.b };
	}

	namespace colors
	{
		static ColorRGB Red{ 1,0,0 };
		static ColorRGB Blue{ 0,0,1 };
		static ColorRGB Green{ 0,1,0 };
		static ColorRGB Yellow{ 1,1,0 };
		static ColorRGB Cyan{ 0,1,1 };
		static ColorRGB Magenta{ 1,0,1 };
		static ColorRGB White{ 1,1,1 };
		static ColorRGB Black{ 0,0,0 };
		static ColorRGB Gray{ 0.5f,0.5f,0.5f };
	}
}

#endif // !COLORRGB_H