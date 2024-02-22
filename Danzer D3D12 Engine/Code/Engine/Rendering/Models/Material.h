#pragma once

struct Material {
	//*Textures ID's, only use TextureHandler  
	//*when assigning these values! 
	unsigned int m_albedo    = 0;
	unsigned int m_normal	 = 0;

	unsigned int m_metallicMap  = 0;
	unsigned int m_roughnessMap = 0;
	unsigned int m_heightMap    = 0;
	unsigned int m_aoMap        = 0;

	//*Additional or replacement values 
	float m_shininess = 1.f;
	float m_roughness = 1.f;
	float m_emissvie  = 1.f;
	float m_color[4]  = {1.f, 1.f, 1.f, 1.f};
};