#pragma once
#include "Rendering/Screen Rendering/FullscreenTexture.h"

class SSAOBlur : public FullscreenTexture
{
public:
	SSAOBlur();

	void RenderTexture();
};

