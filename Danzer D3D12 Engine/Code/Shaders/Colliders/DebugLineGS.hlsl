#include "DebugLineHeader.hlsli"

[maxvertexcount(2)]
void main(
	point VertexToGeometry input[1], 
	inout LineStream<GeometryToPixel> output
)
{
    float4 pos[2];
	
    float4 toWorld = mul(input[0].m_start, view);
	float4 toProj  = mul(toWorld, projection);
    pos[0] = toProj;
	
    toWorld = mul(input[0].m_end, view);
    toProj  = mul(toWorld, projection);
    pos[1] = toProj;
	
	for (uint i = 0; i < 2; i++)
	{
        GeometryToPixel element;
		element.m_position = pos[i];
        element.m_color	   = input[0].m_color;
		output.Append(element);
	}
	
    output.RestartStrip();
}