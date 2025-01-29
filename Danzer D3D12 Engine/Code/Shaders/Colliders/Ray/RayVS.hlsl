#include "RayStructs.hlsli"

VertexToGeometry main( InputToVertex input) 
{
    VertexToGeometry returnValue;
	
    returnValue.m_start = input.m_start;
    returnValue.m_end   = input.m_end;
    returnValue.m_color = input.m_color;
    
	return returnValue;
}