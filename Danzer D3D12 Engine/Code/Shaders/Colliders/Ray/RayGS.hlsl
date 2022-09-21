#include "../ColliderStructs.hlsli"
#include "RayStructs.hlsli"

[maxvertexcount(2)]
void main(
	point VertexToGeometry input[1] , 
	inout LineStream<GeometryToPixel> output
)
{	
    float4 pos[2];
    
    float4 worldPos = mul(CameraTransform,  input[0].m_position);
    float4 projPos  = mul(CameraProjection, worldPos);
    
    pos[0] = projPos;
    
    worldPos = mul(CameraTransform , input[0].m_destination);
    projPos  = mul(CameraProjection, worldPos);
    
    pos[1] = projPos;
    
    for (int i = 0; i < 2; i++)
    {
        GeometryToPixel element;
        element.m_position = pos[i];
        element.m_color = float4(1.f, 0.f, 0.f, 1.f);
        output.Append(element);
    }
    
    output.RestartStrip();
}