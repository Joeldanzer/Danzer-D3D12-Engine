#include "defaultStruct.hlsli"

VertexToPixel main( InputToVertex input )
{
    VertexToPixel returnValue;
    
    float4 toWorld = mul(input.m_transform, input.m_position);
    float4 toCamera = mul(CameraTransform, toWorld );
    float4 toProj = mul(CameraProjection, toCamera);
   
    returnValue.m_position = toProj.xyww;
    returnValue.m_worldPosition = input.m_position;
    returnValue.m_uv = input.m_uv;
    
	return returnValue;
}