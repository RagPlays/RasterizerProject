
// SDL includes
#include <SDL_image.h>
#include <SDL_surface.h>

// Standard includes
#include<string>

// Project includes
#include "ColorRGB.h"
#include "Texture.h"
#include "Vector2.h"

namespace dae
{
	Texture::Texture(SDL_Surface* pSurface) 
		: m_pSurface{ pSurface },
		m_pSurfacePixels{ static_cast<uint32_t*>(pSurface->pixels) }
	{
	}

	Texture::~Texture()
	{
		if (m_pSurface)
		{
			SDL_FreeSurface(m_pSurface);
			m_pSurface = nullptr;
		}
	}

	Texture* Texture::LoadFromFile(const std::string& path)
	{
		return new Texture{ IMG_Load(path.c_str()) };
	}

	const ColorRGB Texture::Sample(const Vector2& uv) const
	{
		const uint32_t index{ static_cast<uint32_t>(uv.x * m_pSurface->w) + static_cast<uint32_t>(uv.y * m_pSurface->h) * m_pSurface->w };
		SDL_Color color;
		SDL_GetRGB(m_pSurfacePixels[index], m_pSurface->format, &color.r, &color.g, &color.b);
		return color;
	}
}