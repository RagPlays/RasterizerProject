
//External includes
#include <iostream>
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Maths.h"
#include "Texture.h"
#include "Utils.h"
#include "BRDFs.h"

using namespace dae;

Renderer::Renderer(SDL_Window* pWindow, int width, int height) 
	: m_pWindow{ pWindow }, 
	m_Width{ width }, 
	m_Height{ height }, 
	m_NrOfPixels{ width * height }
{
	//Create Buffers
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

	// make / fill depthBuffer with FLT_MAX values
	m_pDepthBufferPixels = new float[m_Width * m_Height];
	std::fill_n(m_pDepthBufferPixels, m_NrOfPixels, FLT_MAX);

	// set every pixel to black
	std::fill_n(m_pBackBufferPixels, m_NrOfPixels, uint32_t(0));

	//Initialize Camera
	m_Camera.Initialize(45.f, { 0.f, 5.f, -64.f }, width / (float)height);

	CreateScene();
}

Renderer::~Renderer()
{
	// depthBuffer
	if (m_pDepthBufferPixels) delete[] m_pDepthBufferPixels;

	// textures
	if (m_pDiffuseTexture) delete m_pDiffuseTexture;
	if (m_pNormalMapTexture) delete m_pNormalMapTexture;
	if (m_pGlossTexture) delete m_pGlossTexture;
	if (m_pSpecularTexture) delete m_pSpecularTexture;
}

void dae::Renderer::CreateScene()
{
	// create mesh
	m_TriangleListMesh.primitiveTopology = PrimitiveTopology::TriangleList;
	Utils::ParseOBJ("Resources/vehicle.obj", m_TriangleListMesh.vertices, m_TriangleListMesh.indices);
	m_MeshTranslationMatrix = Matrix::CreateTranslation(0.f, 0.f, 0.f);
	m_MeshRotationMatrix = Matrix::CreateRotation(0.f, 0.f, 0.f);
	m_TriangleListMesh.worldMatrix = m_MeshRotationMatrix * m_MeshTranslationMatrix;

	// Textures
	m_pDiffuseTexture = Texture::LoadFromFile("Resources/vehicle_diffuse.png");
	m_pNormalMapTexture = Texture::LoadFromFile("Resources/vehicle_normal.png");
	m_pGlossTexture = Texture::LoadFromFile("Resources/vehicle_gloss.png");
	m_pSpecularTexture = Texture::LoadFromFile("Resources/vehicle_specular.png");

	// check textures
	assert(m_pDiffuseTexture != nullptr);
	assert(m_pNormalMapTexture != nullptr);
	assert(m_pGlossTexture != nullptr);
	assert(m_pSpecularTexture != nullptr);
}

void Renderer::Update(Timer* pTimer)
{
	m_Camera.Update(pTimer);

	std::fill_n(m_pDepthBufferPixels, m_NrOfPixels, FLT_MAX);
	SDL_FillRect(m_pBackBuffer, NULL, SDL_MapRGB(m_pBackBuffer->format, 100, 100, 100));

	if (m_MeshRotating)
	{
		m_MeshRotateAngle += pTimer->GetElapsed();
		m_MeshRotationMatrix = Matrix::CreateRotation(0.f, m_MeshRotateAngle, 0.f);
		m_TriangleListMesh.worldMatrix = m_MeshRotationMatrix * m_MeshTranslationMatrix;
	}

	VertexTransformationFunction(m_TriangleListMesh.vertices, m_TriangleListMesh.vertices_out);
}

void Renderer::Render() const
{
	//Lock BackBuffer
	SDL_LockSurface(m_pBackBuffer);

	// render the mesh
	RenderListMesh(m_TriangleListMesh);

	//Update SDL Surface
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}


void dae::Renderer::RenderListMesh(const Mesh& listMesh) const
{
	constexpr size_t nrTrianglePoints{ 3 };
	const std::vector<uint32_t>& indices{ listMesh.indices };
	assert(indices.size() % 3 == 0);

	for (size_t index{}; index < indices.size(); index += nrTrianglePoints)
	{
		// store vertices in local variables
		const Vertex_Out& vertex0{ listMesh.vertices_out[indices[index]] };
		const Vertex_Out& vertex1{ listMesh.vertices_out[indices[index + 1]] };
		const Vertex_Out& vertex2{ listMesh.vertices_out[indices[index + 2]] };

		// render triangle with current vertices
		RenderTriangle(vertex0, vertex1, vertex2);
	}
}

void dae::Renderer::RenderStripMesh(const Mesh& stripMesh) const
{
	const std::vector<uint32_t>& indices{ stripMesh.indices };
	assert(indices.size() > 2);
	const size_t maxIndicesSize{ indices.size() - 2 };

	for (size_t index{}; index < maxIndicesSize; ++index)
	{
		const uint32_t idx0{ indices[index] };
		const uint32_t idx1{ indices[index + 1] };
		const uint32_t idx2{ indices[index + 2] };

		if (idx0 == idx1 || idx1 == idx2) continue;

		const Vertex_Out& v0{ stripMesh.vertices_out[idx0] };
		const Vertex_Out& v1{ stripMesh.vertices_out[idx1] };
		const Vertex_Out& v2{ stripMesh.vertices_out[idx2] };

		if (index & 1) // odd
		{
			RenderTriangle(v2, v1, v0);
		}
		else // even
		{
			RenderTriangle(v0, v1, v2);
		}
	}
}



void dae::Renderer::RenderTriangle(const Vertex_Out& vertex0, const Vertex_Out& vertex1, const Vertex_Out& vertex2) const
{
	constexpr int boundingOffset{ 5 };

	// frustum culling left right up down
	if (
		// x
		vertex0.position.x < 0.f || vertex0.position.x > m_Width ||
		vertex1.position.x < 0.f || vertex1.position.x > m_Width ||
		vertex2.position.x < 0.f || vertex2.position.x > m_Width ||

		// y
		vertex0.position.y < 0.f || vertex0.position.y > m_Height ||
		vertex1.position.y < 0.f || vertex1.position.y > m_Height ||
		vertex2.position.y < 0.f || vertex2.position.y > m_Height
		) return;

	const Vector2 vec0{ vertex0.position.GetXY() };
	const Vector2 vec1{ vertex1.position.GetXY() };
	const Vector2 vec2{ vertex2.position.GetXY() };

	const int xMin{ std::max(static_cast<int>(std::min({ vec0.x, vec1.x, vec2.x }) - boundingOffset), 0) };
	const int xMax{ std::min(static_cast<int>(std::max({ vec0.x, vec1.x, vec2.x }) + boundingOffset), m_Width) };
	const int yMin{ std::max(static_cast<int>(std::min({ vec0.y, vec1.y, vec2.y }) - boundingOffset), 0) };
	const int yMax{ std::min(static_cast<int>(std::max({ vec0.y, vec1.y, vec2.y }) + boundingOffset), m_Height) };

	if (xMax < 0 || xMin > m_Width || yMax < 0 || yMin > m_Height) return;

	const Vector2 edge0{ vec2 - vec1 };
	const Vector2 edge1{ vec0 - vec2 };
	const Vector2 edge2{ vec1 - vec0 };

	const float divideW0{ 1.f / vertex0.position.w };
	const float divideW1{ 1.f / vertex1.position.w };
	const float divideW2{ 1.f / vertex2.position.w };

	const float divideZ0{ 1.f / vertex0.position.z };
	const float divideZ1{ 1.f / vertex1.position.z };
	const float divideZ2{ 1.f / vertex2.position.z };

	const Vector2 uv0{ vertex0.uv * divideW0 };
	const Vector2 uv1{ vertex1.uv * divideW1 };
	const Vector2 uv2{ vertex2.uv * divideW2 };

	ColorRGB pixelColor;

	for (int py{ yMin }; py < yMax; ++py)
	{
		for (int px{ xMin }; px < xMax; ++px)
		{
			const Vector2 pixelPoint{ px + 0.5f, py + 0.5f };

			float w0{ Vector2::Cross(pixelPoint - vec1, edge0) };
			float w1{ Vector2::Cross(pixelPoint - vec2, edge1) };
			float w2{ Vector2::Cross(pixelPoint - vec0, edge2) };

			if (w0 <= 0.f && w1 <= 0.f && w2 <= 0.f)
			{
				const float invTotalWeight{ 1.f / (w0 + w1 + w2) };
				w0 *= invTotalWeight;
				w1 *= invTotalWeight;
				w2 *= invTotalWeight;

				const float interPolatedZ{ 1.f / (divideZ0 * w0 + divideZ1 * w1 + divideZ2 * w2) }; // (depthValue)
				const int pixelIdx{ px + (py * m_Width) };

				if (interPolatedZ >= 0.f && interPolatedZ <= 1.f && m_pDepthBufferPixels[pixelIdx] >= interPolatedZ)
				{
					const float interPolatedW{ 1.f / (divideW0 * w0 + divideW1 * w1 + divideW2 * w2) };
					const Vector2 uvInterPolated{ (uv0 * w0 + uv1 * w1 + uv2 * w2) * interPolatedW };

					if (uvInterPolated.x < 0 || uvInterPolated.x > 1.f || uvInterPolated.y < 0 || uvInterPolated.y > 1.f) return;

					m_pDepthBufferPixels[pixelIdx] = interPolatedZ;

					const Vertex_Out shadeVertex
					{
						{
							static_cast<float>(px),
							static_cast<float>(py),
							interPolatedZ,
							interPolatedW
						},
						ColorRGB{},
						uvInterPolated,
						(vertex0.normal * w0 + vertex1.normal * w1 + vertex2.normal * w2).Normalized(),
						(vertex0.tangent * w0 + vertex1.tangent * w1 + vertex2.tangent * w2).Normalized(),
						(vertex0.viewDirection * w0 + vertex1.viewDirection * w1 + vertex2.viewDirection * w2).Normalized()
					};

					if (m_MeshDepthBuffer)
					{
						pixelColor = Remap(interPolatedZ, 0.985f, 1.f);
					}
					else
					{
						PixelShading(shadeVertex, pixelColor);
					}

					pixelColor.MaxToOne();

					m_pBackBufferPixels[pixelIdx] = SDL_MapRGB
					(
						m_pBackBuffer->format,
						static_cast<uint8_t>(pixelColor.r * 255.f),
						static_cast<uint8_t>(pixelColor.g * 255.f),
						static_cast<uint8_t>(pixelColor.b * 255.f)
					);
				}
			}
		}
	}	
}

void dae::Renderer::PixelShading(const Vertex_Out& v, ColorRGB& pixelColor) const
{
	// shading values
	constexpr float lightIntensity{ 7.f };
	const Vector3 lightDirection{ 0.577f, -0.577f, 0.577f }; // directional light
	constexpr float shininess{ 25.f };
	const ColorRGB ambient{ 0.03f, 0.03f, 0.03f };

	pixelColor = ambient;

	Vector3 normal{};
	if (m_MeshNormalMap)
	{
		//binormal
		const Vector3 binormal{ Vector3::Cross(v.normal, v.tangent) };
		const Matrix tangentSpaceAxis{ v.tangent, binormal, v.normal, Vector3::Zero };

		ColorRGB sampledNormalColor{ m_pNormalMapTexture->Sample(v.uv) };
		sampledNormalColor *= 2.f;
		sampledNormalColor -= 1.f;

		normal = { sampledNormalColor.r, sampledNormalColor.g, sampledNormalColor.b };
		normal = tangentSpaceAxis.TransformVector(normal).Normalized();
	}
	else
	{
		normal = v.normal;
	}

	// observed Area
	const float observedArea{ Vector3::Dot(normal, -lightDirection) };
	if (observedArea < 0.f) return;

	// lambert
	const ColorRGB lambert{ BRDF::Lambert(m_pDiffuseTexture->Sample(v.uv), lightIntensity) };

	// phong
	const ColorRGB specularColor{ m_pSpecularTexture->Sample(v.uv) };
	const float glossiness{ (m_pGlossTexture->Sample(v.uv)).r * shininess };
	const ColorRGB specular{ BRDF::Phong(specularColor, glossiness , -lightDirection, v.viewDirection, normal) };

	switch (m_MeshShadingMode)
	{
	case ShadingMode::observedArea:
		pixelColor += observedArea;
		return;
	case ShadingMode::diffused: // (incl OA)
		pixelColor += lambert * observedArea;
		return;
	case ShadingMode::specular: // (incl OA)
		pixelColor += specular * observedArea;
		return;
	case ShadingMode::combined:
		pixelColor += (lambert + specular) * observedArea;
		return;
	default:
		assert(false);
		return;
	}

	return;
}

void Renderer::VertexTransformationFunction(const std::vector<Vertex>& vertices_in, std::vector<Vertex_Out>& vertices_out)
{
	if (vertices_in.empty()) return;				// make sure vertices_in isnt empty

	// vertices_in are in WORLD space -> need them in screen space -> vertices_out

	// World space to View space to Porjection space to Screen space

	// Reserve same amount for vertices_out
	if (vertices_out.empty()) vertices_out.resize(vertices_in.size());
	
	const float halfWidth{ m_Width * 0.5f};
	const float halfHeight{ m_Height * 0.5f};

	for (size_t idx{}; idx < vertices_out.size(); ++idx)
	{
		// change uv
		vertices_out[idx].uv = vertices_in[idx].uv;

		// change color
		vertices_out[idx].color = vertices_in[idx].color;

		// change normal
		vertices_out[idx].normal = m_TriangleListMesh.worldMatrix.TransformVector(vertices_in[idx].normal); // update normal only with worldMatrix

		// change tangent
		vertices_out[idx].tangent = m_TriangleListMesh.worldMatrix.TransformPoint(vertices_in[idx].tangent);

		// WorldViewProjectionMatrix 
		const Matrix worldViewProjectionMatrix{ m_TriangleListMesh.worldMatrix * m_Camera.viewMatrix * m_Camera.projectionMatrix };

		// set in vertices out
		vertices_out[idx].position = worldViewProjectionMatrix.TransformPoint({ vertices_in[idx].position, vertices_in[idx].position.z });

		// set viewDirection
		vertices_out[idx].viewDirection = worldViewProjectionMatrix.TransformPoint(vertices_in[idx].position).Normalized();

		// divide
		const float invW{ 1.f / vertices_out[idx].position.w };
		vertices_out[idx].position.x *= invW;
		vertices_out[idx].position.y *= invW;
		vertices_out[idx].position.z *= invW;

		// putting it in screen space
		vertices_out[idx].position.x = (vertices_out[idx].position.x + 1.f) * halfWidth;
		vertices_out[idx].position.y = (1.f - vertices_out[idx].position.y) * halfHeight;
	}
}

void dae::Renderer::ToggleDepthBuffer()
{
	m_MeshDepthBuffer = !m_MeshDepthBuffer;
	if (m_MeshDepthBuffer)
	{
		std::cout << "DepthBufferColor: ON\n";
	}
	else
	{
		std::cout << "DepthBufferColor: OFF\n";
	}
}

void dae::Renderer::ToggleRotation()
{
	m_MeshRotating = !m_MeshRotating;
	if (m_MeshRotating)
	{
		std::cout << "Rotation: ON\n";
	}
	else
	{
		std::cout << "Rotation: OFF\n";
	}
}

void dae::Renderer::ToggleNormalMap()
{
	m_MeshNormalMap = !m_MeshNormalMap;
	if (m_MeshNormalMap)
	{
		std::cout << "Using Normal Map: ON\n";
	}
	else
	{
		std::cout << "Using Normal Map: OFF\n";
	}
}

void dae::Renderer::CycleShadingMode()
{
	switch (m_MeshShadingMode)
	{
	case dae::Renderer::ShadingMode::observedArea:
		m_MeshShadingMode = ShadingMode::diffused;
		std::cout << "ShadingMode: Diffused\n";
		break;
	case dae::Renderer::ShadingMode::diffused:
		m_MeshShadingMode = ShadingMode::specular;
		std::cout << "ShadingMode: Specular\n";
		break;
	case dae::Renderer::ShadingMode::specular:
		m_MeshShadingMode = ShadingMode::combined;
		std::cout << "ShadingMode: Combined\n";
		break;
	case dae::Renderer::ShadingMode::combined:
		m_MeshShadingMode = ShadingMode::observedArea;
		std::cout << "ShadingMode: observedArea\n";
		break;
	default:
		assert(false);
		break;
	}
}

float dae::Renderer::Remap(float v, float min, float max) const
{
	return std::clamp((v - min) / (max - min), 0.f, 1.f);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}