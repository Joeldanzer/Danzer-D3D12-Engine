struct GeometryToPixel
{
    float4 m_position : SV_Position;
    float4 m_color    : COLOR;
};

cbuffer MainCamera : register(b0)
{
    float4x4 CameraTransform;
    float4x4 CameraProjection;
    float4 CameraPosition; 
}

sampler defaultSampler : register(s0);