#pragma once

struct Material {
	// Textures ID's, only use TextureHandler or ModelHandler 
	// when assigning these values! 
	unsigned int m_albedo;
	unsigned int m_normal;
	unsigned int m_metallic;

	// Additional or replacement values 
	float m_shininess;
	float m_roughness;
	float m_emissvie;
	float m_color[4] = {1.f, 1.f, 1.f, 1.f};
};