#include "defaultStruct.hlsli"

VertexToPixel main(InputToVertex input)
{
    VertexToPixel returnValue;
    
    // From object -> world -> camera -> projectioc
    float4 toWorld =  mul(input.m_transform, input.m_position);
    float4 toCamera = mul(CameraTransform, toWorld );
    float4 toProj =   mul(CameraProjection, toCamera);
    
    returnValue.m_position = toProj;
    returnValue.m_worldPosition = toWorld;
    returnValue.m_uv = input.m_uv;
	return returnValue;
}