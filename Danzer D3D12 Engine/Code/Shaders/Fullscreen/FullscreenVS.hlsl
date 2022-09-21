#include "FullscreenHeader.hlsli"

VertexToPixel main(InputToVertex input)
{
    float4 positions[3] =
    {
        float4(-1.0f, -1.0f, 0.0f, 1.0f),
        float4(-1.0f, 3.0f, 0.0f, 1.0f),
        float4(3.0f, -1.0f, 0.0f, 1.0f)
    };
    
    float2 uv[3] =
    {
        float2(0.0f, 1.0f),
        float2(0.0f, -1.0f),
        float2(2.0f, 1.0f)
    };
    
    VertexToPixel output;
    output.m_position = positions[input.m_vertexID];
    output.m_uv = uv[input.m_vertexID];
    return output;
}
