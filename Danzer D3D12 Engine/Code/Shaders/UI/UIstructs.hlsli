struct InputToVertex
{
    float4 m_position : POSITION;
    float2 m_uv       : UV;

    float2 m_sheetSize     : SHEET_SIZE;
    float2 m_framePosition : FRAME_POSITION;
    float2 m_frameAnchor   : FRAME_ANCHOR;
    float2 m_frameSize     : FRAME_SIZE;
    float2 m_uiSize        : UI_SIZE;
};

struct VertexToPixel
{
    float4 m_position : SV_Position;
    float2 m_uv       : UV;
};

cbuffer ScreenSize : register(b0)
{
    float2 Screen;
}

SamplerState defaultSampler : register(s0);
Texture2D    uiTexture      : register(t0);
Texture2D    uiTexture2      : register(t1);