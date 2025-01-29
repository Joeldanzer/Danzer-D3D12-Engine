#include "AABBStructs.hlsli"

VertexToPixel main( InputToVertex input ) 
{
    VertexToPixel returnValue;
	
    float4 toWorld = mul(input.m_position, input.m_transform);
    float4 toCam   = mul(toWorld, camera);
    float4 toProj  = mul(toCam, projection);
    
    returnValue.m_position = toProj;
    returnValue.m_color    = input.m_color;
    
	return returnValue;
}