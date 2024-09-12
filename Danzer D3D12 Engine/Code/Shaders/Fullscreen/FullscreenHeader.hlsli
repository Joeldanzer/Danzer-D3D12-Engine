struct InputToVertex
{
    unsigned int m_vertexID : SV_VertexID;
};

struct VertexToPixel
{
    float4 m_position : SV_Position;
    float2 m_uv       : UV;
};

cbuffer DefaultBuffer : register(b0)
{
    float4x4 CameraProjection;
    float4x4 CameraTransform;
    float4 CameraPosition;
    float4 Eye;
    
    float Time;
    float ScreenWidth;
    float ScreenHeight;
    
    // Needs to be 256-byte alligned
    float trashOne;
    float4 trashTwo[5];
}