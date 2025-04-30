#pragma once

struct Material {
	//*Textures ID's, only use TextureHandler  
	//*when assigning these values! 
	uint32_t m_albedo       = UINT32_MAX;
	uint32_t m_normal	    = UINT32_MAX;
	uint32_t m_metallicMap  = UINT32_MAX;
	uint32_t m_roughnessMap = UINT32_MAX;
	uint32_t m_heightMap    = UINT32_MAX;
	uint32_t m_aoMap        = UINT32_MAX;

	//*Additional or replacement values 
	float m_shininess = 1.f;
	float m_roughness = 1.f;
	float m_emissvie  = 1.f;
	float m_color[4]  = {1.f, 1.f, 1.f, 1.f};
};