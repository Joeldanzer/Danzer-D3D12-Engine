struct LightOutput
{
    float4 m_sceneColor  : SV_TARGET0;
    float4 m_brightColor : SV_TARGET1;
};

SamplerState defaultSample       : register(s0);

Texture2D   albedoTexture        : register(t0);
Texture2D   normalTexture        : register(t1);
Texture2D   materialTexture      : register(t2);
Texture2D   vertexColorTexture   : register(t3);
Texture2D   vertexNormalTexture  : register(t4);
Texture2D   worldPositionTexture : register(t5);
Texture2D   depthTexture         : register(t6);
TextureCube skyboxTexture        : register(t7);
Texture2D   shadowMap            : register(t8);
Texture2D   ssaoTexture          : register(t9);
Texture2D   volumetricLight      : register(t10);