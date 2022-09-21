#include "AABBStructs.hlsli"

VertexToGeometry main( InputToVertex input ) 
{
    VertexToGeometry returnValue;
	
    returnValue.m_position = float4(input.m_position, 1.f);
    returnValue.m_size = input.m_size;
    
	return returnValue;
}