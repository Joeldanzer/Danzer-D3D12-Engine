#pragma once
#include "Rendering/Buffers/VertexBuffer.h"
#include "Core/MathDefinitions.h"

class D3D12Framework;

class  Transform;
struct PointLight;
struct SpotLight;

class LightHandler
{
public:
	LightHandler(D3D12Framework& framework);
	~LightHandler();

	void AddLightInstanceForRendering(const PointLight& light, const Transform& transform);
	void AddLightInstanceForRendering(const SpotLight&  light, const Transform& transform);
	
	void UpdateLightInstances(const uint8_t frameIndex);

	void ClearAllInstances();
private:
	friend class LightRenderer;

	struct PointLightData {
		Vect4f m_position;
		Vect4f m_color;
		float  m_radius;
	};

	struct SpotLightData {
	
	};

	std::vector<PointLightData> m_pointLightInstances;
	std::vector<SpotLightData>  m_spotLightInstances;

	VertexBuffer m_pointLightBuffer;
	VertexBuffer m_spotLightBuffer;
};

