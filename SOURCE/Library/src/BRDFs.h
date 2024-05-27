#pragma once
#include "Math.h"
#include "ColorRGB.h"
#include "Vector3.h"

namespace dae
{
	namespace BRDF
	{
		/**
		 * \param kd Diffuse Reflection Coefficient
		 * \param cd Diffuse Color
		 * \return Lambert Diffuse Color
		 */
		static ColorRGB Lambert(float kd, const ColorRGB& cd)
		{
			return { cd * kd * DIV_PI };
		}

		static ColorRGB Lambert(const ColorRGB& kd, const ColorRGB& cd)
		{
			return { cd * kd * DIV_PI };
		}

		/**
		 * \brief todo
		 * \param ks Specular Reflection Coefficient
		 * \param exp Phong Exponent
		 * \param l Incoming (incident) Light Direction
		 * \param v View Direction
		 * \param n Normal of the Surface
		 * \return Phong Specular Color
		 */
		static ColorRGB Phong(const ColorRGB& ks, float exp, const Vector3& l, const Vector3& v, const Vector3& n)
		{
			const float cosA{ Vector3::Dot(Vector3::Reflect(l, n), v) };

			if (cosA < FLT_EPSILON) return {};

			return ks * powf(cosA, exp);
		}
	}
}