#include "AABBStructs.hlsli"
#include "../ColliderStructs.hlsli"

static const int INDICES[36] =
{
    0, 2, 3,
    0, 3, 1,
    0, 4, 5,
    0, 5, 1,

    0, 4, 6,
    0, 6, 2,

    2, 6, 7,
    2, 7, 3,

    1, 5, 7,
    1, 7, 3,

    4, 6, 7,
    4, 7, 5
};
static const float3 POSITIONS[8] =
{
    float3(-1.0f, -1.0f,  1.0f),
    float3(-1.0f,  1.0f,  1.0f),
    float3( 1.0f, -1.0f,  1.0f),
    float3( 1.0f,  1.0f,  1.0f),
    float3(-1.0f, -1.0f, -1.0f),
    float3(-1.0f,  1.0f, -1.0f),
    float3( 1.0f, -1.0f, -1.0f),
    float3( 1.0f,  1.0f, -1.0f)
};

static const float3 NORMAL[3] =
{
    float3(1.f, 0.f, 0.f),
    float3(0.f, 1.f, 0.f),
    float3(0.f, 0.f, 1.f)
};

[maxvertexcount(36)]
void main(
	point VertexToGeometry input[1], 
	inout TriangleStream<GeometryToPixel> output
)
{    
	for (uint i = 0; i < 36; i++)
	{
		GeometryToPixel element;
        
        float4 objPos = input[0].m_position;
        objPos.x += POSITIONS[INDICES[i]].x > 0.f ? input[0].m_size.x / 2.f : -(input[0].m_size.x / 2.f);
        objPos.y += POSITIONS[INDICES[i]].y > 0.f ? input[0].m_size.y / 2.f : -(input[0].m_size.y / 2.f);
        objPos.z += POSITIONS[INDICES[i]].z > 0.f ? input[0].m_size.z / 2.f : -(input[0].m_size.z / 2.f);
        
        float4 worldPos = mul(CameraTransform, objPos);
        float4 projPos = mul(CameraProjection, worldPos);
        
		element.m_position = projPos;
        element.m_color = float4(0.f, 1.f, 0.f, 1.f);
        output.Append(element);
	}
    
    output.RestartStrip();
}