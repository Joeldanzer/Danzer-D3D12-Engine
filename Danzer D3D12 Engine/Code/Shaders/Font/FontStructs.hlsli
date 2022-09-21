
struct InputToVertex
{
    float4 m_fontPosition : POSITION;
    float2 m_fontSize     : FONT_SIZE; 
    float2 m_sheetSize    : SHEET_SIZE;
    float2 m_newPositon   : NEW_POSITION; 
    float2 m_size         : SIZE;
    float2 m_anchor       : ANCHOR;
    float4 m_color        : COLOR;
};

struct VertexToGeometry
{
    float4 m_fontPosition : SV_Position;
    float4 m_color        : COLOR;
    float2 m_sheetSize    : SHEET_SIZE;
    float2 m_fontSize     : FONT_SIZE;
    float2 m_newPositon   : NEW_POSITION;
    float2 m_size         : SIZE;
    float2 m_anchor       : ANCHOR;
};

struct GeometryToPixel
{
    float4 m_position : SV_Position;
    float4 m_color    : COLOR;
    float2 m_UV       : UV;
};

cbuffer ScreenSize : register(b0)
{
    float2 Screen;
}



SamplerState defaultSampler : register(s0);
Texture2D    fontTexture    : register(t0);