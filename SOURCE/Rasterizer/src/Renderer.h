
#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include "Camera.h"
#include "DataTypes.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	struct Vertex_Out;
	struct Vertex;
	struct Mesh;

	class Texture;
	class Timer;
	class Scene;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow, int width, int height);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void CreateScene();

		void Update(Timer* pTimer);
		void Render() const;
		void RenderListMesh(const Mesh& listMesh) const;
		void RenderStripMesh(const Mesh& stripMesh) const;
		void RenderTriangle(const Vertex_Out& vertex0, const Vertex_Out& vertex1, const Vertex_Out& vertex2) const;

		void PixelShading(const Vertex_Out& v, ColorRGB& color) const;

		bool SaveBufferToImage() const;

		void VertexTransformationFunction(const std::vector<Vertex>& vertices_in, std::vector<Vertex_Out>& vertices_out);

		void ToggleDepthBuffer();
		void ToggleRotation();
		void ToggleNormalMap();
		void CycleShadingMode();


		float Remap(float v, float min, float max) const;

	private:
		SDL_Window* m_pWindow;

		SDL_Surface* m_pFrontBuffer;
		SDL_Surface* m_pBackBuffer;
		uint32_t* m_pBackBufferPixels;

		Mesh m_TriangleListMesh;
		Matrix m_MeshTranslationMatrix{};
		Matrix m_MeshRotationMatrix{};
		float m_MeshRotateAngle{};

		Texture* m_pDiffuseTexture;
		Texture* m_pNormalMapTexture; 
		Texture* m_pGlossTexture;
		Texture* m_pSpecularTexture;

		float* m_pDepthBufferPixels;

		Camera m_Camera{};

		const int m_Width;
		const int m_Height;
		const int m_NrOfPixels;

		// inputs
		enum class ShadingMode
		{
			observedArea = 0,
			diffused,
			specular,
			combined
		};
		bool m_MeshDepthBuffer{ false };
		bool m_MeshRotating{ true };
		bool m_MeshNormalMap{ true };
		ShadingMode m_MeshShadingMode{ ShadingMode::combined };
	};
}

#endif // !RENDERER_H