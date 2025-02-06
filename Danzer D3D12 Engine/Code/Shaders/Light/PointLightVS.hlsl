#include "PointLightHeader.hlsli"

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

VertexToPixel main(InputToVertex input, uint vertexID : SV_VertexID)
{
    VertexToPixel returnValue;
    
    returnValue.m_LightPosition = input.m_position; // Might need to multiply with camera & proj? 
    returnValue.m_color         = input.m_color;
    returnValue.m_radius        = input.m_radius;	
    returnValue.m_svPosition    = positions[vertexID];
    returnValue.m_uv            = uv[vertexID];
    
    return returnValue;
}