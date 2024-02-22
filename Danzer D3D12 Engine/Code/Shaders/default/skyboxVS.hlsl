#include "defaultStruct.hlsli"

VertexToPixel main( InputToVertex input )
{
    VertexToPixel returnValue;
    
    float4 toWorld  = mul(input.m_position, input.m_transform);
    float4 toCamera = mul(toWorld,  CameraTransform);
    float4 toProj   = mul(toCamera, CameraProjection);
   
    returnValue.m_position      = toProj.xyww;
    returnValue.m_worldPosition = input.m_position;
    returnValue.m_uv            = input.m_uv;
    
	return returnValue;
}