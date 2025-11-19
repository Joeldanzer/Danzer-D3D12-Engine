#pragma once

typedef uint32_t MaterialID;

struct Material {
	uint32_t m_albedo       = UINT32_MAX;
	uint32_t m_normal	    = UINT32_MAX;
	uint32_t m_metallicMap  = UINT32_MAX;
	uint32_t m_roughnessMap = UINT32_MAX;
	uint32_t m_heightMap    = UINT32_MAX;
	uint32_t m_aoMap        = UINT32_MAX;

	//* Additional or replacement values 
	float m_shininess = 1.f;
	float m_roughness = 1.f;
	float m_emissvie  = 1.f;
	float m_color[4]  = {1.f, 1.f, 1.f, 1.f};
};

class MaterialHandler {
public:
	MaterialHandler();
	~MaterialHandler();

	//MaterialID CreateMaterial(
	//	const uint32_t alb, const uint32_t nor, const uint32_t met, const uint32_t rou, const uint32_t hei, const uint32_t ao,
	//	const float shi = 1.0f, const float addRou = 1.0f, const float emi = 1.0f);
};