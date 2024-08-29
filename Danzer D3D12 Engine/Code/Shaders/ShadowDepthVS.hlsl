#include "Gbuffer/GbufferHeader.hlsli"

VertexToPixel main(InputToVertex input)
{
	VertexToPixel output;
	
    float4 toWorld  = mul(input.m_position, input.m_transform);
    float4 toLight  = mul(toWorld, CameraTransform);
    float4 toProj   = mul(toLight, CameraProjection);
	
    //float4x4 lightSpaceMatrix = mul(CameraTransform,   CameraProjection);
    //lightSpaceMatrix		  = mul(input.m_transform, lightSpaceMatrix);
    //float4 position			  = mul(input.m_position,  lightSpaceMatrix);

    output.m_position      = toProj;
	output.m_normal		   = float4(0.0f, 0.0f, 0.0f, 0.0f);
	output.m_tangent       = float4(0.0f, 0.0f, 0.0f, 0.0f);
	output.m_biNormal	   = float4(0.0f, 0.0f, 0.0f, 0.0f);
	output.m_color         = float4(0.0f, 0.0f, 0.0f, 0.0f);
    output.m_worldPosition = float4(0.0f, 0.0f, 0.0f, 0.0f);
	output.m_uv			   = float2(0.0f, 0.0f);

	return output;
}