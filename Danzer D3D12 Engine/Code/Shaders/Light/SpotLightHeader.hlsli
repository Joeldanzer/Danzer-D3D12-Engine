struct InputToVertex
{
    float4 m_position    : POSITION;
    float4 m_color       : COLOR;
    float4 m_direction   : DIRECTION;
    float  m_cutOff      : CUT_OFF;
    float  m_outerCutOff : OUTER_CUT_OFF;
    float  m_range       : RANGE;
    uint   m_vertexID    : SV_VertexID;
};

struct VertexToPixel
{
    float4 m_svPosition  : SV_Position;
    float2 m_uv          : UV;
    float4 m_position    : POSITION;
    float4 m_color       : COLOR;
    float4 m_direction   : DIRECTION;
    float  m_cutOff      : CUT_OFF;
    float  m_outerCutOff : OUTER_CUT_OFF;
    float  m_range       : RANGE;
};

cbuffer CameraBuffer : register(b0)
{
    float4x4 CameraProjection;
    float4x4 CameraTransform;
    float4   CameraPosition;
    
    float4 trash[7];
}