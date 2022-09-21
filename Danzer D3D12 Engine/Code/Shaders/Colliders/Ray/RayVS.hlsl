#include "RayStructs.hlsli"

VertexToGeometry main( InputToVertex input) 
{
    VertexToGeometry returnValue;
	
    returnValue.m_position = float4(input.m_position, 1.f);
    returnValue.m_destination = float4(input.m_destination, 1.f);
	return returnValue;
}