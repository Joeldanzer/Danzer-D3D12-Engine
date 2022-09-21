#include "UIstructs.hlsli"

VertexToPixel main(InputToVertex input)
{
    VertexToPixel returnValue;
    
    // Size of a pixel in screen Units
    const float2 pixelSize = float2(2.f, 2.f) / Screen;

    // UV map
    float2 spriteUV = (input.m_framePosition + input.m_uv * input.m_frameSize) / input.m_sheetSize;

    const float2 pixelPos = (input.m_position.xy * input.m_sheetSize) / 2.f;
    
    // Transform position to -1..+1 xy-space
    const float2 unitQuadCorner = 2.f * pixelPos / input.m_sheetSize;
    
    // Final position in screen space
    float2 screenSpacePos = input.m_frameAnchor;
    screenSpacePos += unitQuadCorner * (0.5f * input.m_frameSize) * pixelSize * input.m_uiSize;
    
    returnValue.m_position.xy = screenSpacePos.xy;
    returnValue.m_position.z = 0.f;
    returnValue.m_position.w = 1.f;
    returnValue.m_uv = spriteUV;
    
    return returnValue;
}