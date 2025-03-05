#include "SpotLightHeader.hlsli"

VertexToPixel main(InputToVertex input)
{
    static float4 positions[3] =
    {
        float4(-1.0f, -1.0f, 0.0f, 1.0f),
        float4(-1.0f, 3.0f, 0.0f, 1.0f),
        float4(3.0f, -1.0f, 0.0f, 1.0f)
    };
    
    static float2 uv[3] =
    {
        float2(0.0f, 1.0f),
        float2(0.0f, -1.0f),
        float2(2.0f, 1.0f)
    };
    
    VertexToPixel output;
    output.m_position    = input.m_position;
    output.m_color       = input.m_color;
    output.m_direction   = input.m_direction;
    output.m_cutOff      = input.m_cutOff;
    output.m_outerCutOff = input.m_outerCutOff;
    output.m_range       = input.m_range;
    output.m_svPosition  = positions[input.m_vertexID];
    output.m_uv          = positions[input.m_vertexID];
    
	return output;
}