#include "../ColliderStructs.hlsli"
#include "RayStructs.hlsli"

[maxvertexcount(2)]
void main(
	point VertexToGeometry input[1], 
	inout LineStream<GeometryToPixel> output
)
{	
    float4 pos[2];
    
    float4 worldPos = mul(CameraTransform,  input[0].m_start);
    float4 projPos  = mul(CameraProjection, worldPos);
    
    pos[0] = projPos;
    
    worldPos = mul(CameraTransform,  input[0].m_end);
    projPos  = mul(CameraProjection, worldPos);
    
    pos[1] = projPos;
    
    for (int i = 0; i < 2; i++)
    {
        GeometryToPixel element;
        element.m_position = pos[i];
        element.m_color    = input[0].m_color;
        output.Append(element);
    }
    
    output.RestartStrip();
}