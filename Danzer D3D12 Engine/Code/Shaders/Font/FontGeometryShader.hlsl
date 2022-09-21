#include "FontStructs.hlsli"


[maxvertexcount(6)]
void main(
	point VertexToGeometry input[1],
	inout TriangleStream<GeometryToPixel> output
)
{
    const float2 pixelSize = float2(2.f, 2.f) / Screen;
	
    // Prest UV's
    float2 UV[6] =
    {
        {0.f, 0.f},
        {1.f, 1.f},
        {0.f, 1.f},
        {0.f, 0.f},
        {1.f, 0.f},
        {1.f, 1.f}, 
    };
    
    float2 pos[6] =
    {
        float2(-1.f, 1.f),
        float2(1.f, -1.f),
        float2(-1.f, -1.f),
        float2(-1.f, 1.f),
        float2(1.f,  1.f),
        float2(1.f, -1.f)
    };
    
	for (uint i = 0; i < 6; i++)
	{
        GeometryToPixel element;
        
        float2 spriteUV = (input[0].m_fontPosition.xy + UV[i] * input[0].m_fontSize) / input[0].m_sheetSize;
        
        const float2 pixelPos = ((pos[i].xy + input[0].m_newPositon.xy) * input[0].m_sheetSize) / 2.f;
        
        const float2 unitQuadCorner = 2.f * pixelPos / input[0].m_sheetSize;
        
        float2 screenSpacePos = input[0].m_anchor;
        screenSpacePos += unitQuadCorner * (0.5f * input[0].m_fontSize) * pixelSize * input[0].m_size;
		
        element.m_position.xy = screenSpacePos;
        element.m_position.zw = float2(0.f, 1.f);
        element.m_UV = spriteUV;
        element.m_color = input[0].m_color;
        
		output.Append(element);
	}
}