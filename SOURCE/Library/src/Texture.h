
#ifndef TEXTURE_H
#define TEXTURE_H

namespace dae
{
	struct Vector2;
	struct ColorRGB;

	class Texture
	{
	public:
		~Texture();

		static Texture* LoadFromFile(const std::string& path);
		const ColorRGB Sample(const Vector2& uv) const;

	private:
		Texture(SDL_Surface* pSurface);

		SDL_Surface* m_pSurface{ nullptr };
		uint32_t* m_pSurfacePixels{ nullptr };
	};
}

#endif // !TEXTURE_H