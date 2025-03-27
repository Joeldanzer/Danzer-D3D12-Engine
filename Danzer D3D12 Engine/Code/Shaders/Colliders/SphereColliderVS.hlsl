#include "SphereColliderHeader.hlsli"

VertexToPixel main(InputToVertex input)
{
    VertexToPixel output;
    float4 toCam  = mul(input.m_position, Camera);
    float4 toProj = mul(toCam, Projection);
    
    output.m_svPosition = toProj;
    output.m_color      = input.m_color;
    output.m_position   = input.m_position;
    output.m_radius     = input.m_radius;
    
	return output;
}