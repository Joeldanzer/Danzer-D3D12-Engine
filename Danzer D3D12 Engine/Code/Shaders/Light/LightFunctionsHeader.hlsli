#define PI_MACRO 3.14159265359f;
#define FLT_EPSILON 1.192092896e-07f
#define nMipOffset 3
    
float3 LinearToGamma(float3 aColor)
{
    return pow(abs(aColor), 1.0 / 2.2);
}

float3 GammaToLinear(float3 aColor)
{
    return pow(abs(aColor), 2.2);
}

float3 ACESFilm(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

float RoughnessFromPerceptualRoughness(float fPerceptualRoughness)
{
    return fPerceptualRoughness * fPerceptualRoughness;
}

float SpecularPowerFromPerceptualRoughness(float fPerceptualRoughness)
{
    float fRoughness = RoughnessFromPerceptualRoughness(fPerceptualRoughness);
    return (2.0 / max(FLT_EPSILON, fRoughness * fRoughness)) - 2.0f;
}

float PerceptualRoughnessFromRoughness(float fRoughness)
{
    return sqrt(max(0.0, fRoughness));
}

float PerceptualRoughnessFromSpecularPower(float fSpecPower)
{
    float fRoughness = sqrt(2.0 / (fSpecPower + 2.0f));
    return PerceptualRoughnessFromRoughness(fRoughness);
}

int GetNumMips(TextureCube cubeTex)
{
    int iWidth = 0, iHeight = 0, numMips = 0;
    cubeTex.GetDimensions(0, iWidth, iHeight, numMips);
    return numMips;
}

float BurleyToMip(float fPerceptualRougness, int nMips, float NdotR)
{
    float fSpecPower = SpecularPowerFromPerceptualRoughness(fPerceptualRougness);
    fSpecPower /= (4 * max(NdotR, FLT_EPSILON));
    float fScale = PerceptualRoughnessFromSpecularPower(fSpecPower);
    return fScale * (nMips - 1 - nMipOffset);
}

float3 GetSpecularDominantDir(float3 vN, float3 vR, float fRealRoughness)
{
    float fInvRealRough = saturate(1 - fRealRoughness);
    float lerpFactor = fInvRealRough * (sqrt(fInvRealRough) + fRealRoughness);
    
    return lerp(vN, vR, lerpFactor);
}

float GetReductionInMicroFacets(float perceptualRoughness)
{
    float roughness = RoughnessFromPerceptualRoughness(perceptualRoughness);
    return 1.0 / (roughness * roughness + 1.0);
}

float bias(float value, float b)
{
    return (b > 0.0) ? pow(abs(value), log(b) / log(0.5)) : 0.0;
}

float gain(float value, float g)
{
    return 0.5 * ((value < 0.5) ? bias(2.0 * value, 1.0 - g) : (2.0 - bias(2.0 - 2.0 * value, 1.0 - g)));
}

float EmpiricalSpecularAO(float ao, float perceptualRoughness)
{
    float fSmooth = 1 - perceptualRoughness;
    float fSpecAo = gain(ao, 0.5 + max(0.0, fSmooth * 0.4));
    
    return min(1.0, fSpecAo + lerp(0.0, 0.5, fSmooth * fSmooth * fSmooth * fSmooth));
}

float ApproximateSpecularSelfOcclusion(float3 vR, float3 vertNormalNormalized)
{
    const float fFadeParam = 1.3;
    float rimmask = clamp(1 + fFadeParam * dot(vR, vertNormalNormalized), 0.0, 1.0);
    rimmask *= rimmask;
    
    return rimmask;
}

float3 Diffuse(float3 pAlbedo)
{
    return pAlbedo / PI_MACRO;
}

float NormalDistribution_GGX(float a, float NdH)
{
    float a2 = a * a;
    float NdH2 = NdH * NdH;
    
    float denominator = NdH2 * (a2 - 1.0f) + 1.0f;
    denominator *= denominator;
    denominator *= PI_MACRO;
    
    return a2 / denominator;
}

float Specular_D(float a, float NdH)
{
    return NormalDistribution_GGX(a, NdH);
}
float Gemoetric_Smith_Schlick_GGX(float a, float NdV, float NdL)
{
    float k = a * 0.5f;
    float GV = NdV / (NdV * (1 - k) + k);
    float GL = NdL / (NdL * (1 - k) + k);
    
    return GV * GL;
}
float Specular_G(float a, float NdV, float NdL, float Ndh, float VdH, float LdV)
{
    return Gemoetric_Smith_Schlick_GGX(a, NdV, NdL);
}
float3 Fresnel_Schlik(float3 specularColor, float3 h, float3 v)
{
    return (specularColor + (1.0f - specularColor) * pow((1.0f - saturate(dot(v, h))), 5));
}
float3 Specular_F(float3 specularColor, float3 h, float3 v)
{
    return Fresnel_Schlik(specularColor, h, v);
}
float3 Specular(float3 specularColor, float3 h, float3 v, float3 l, float a, float NdL, float NdV, float NdH, float VdH, float LdV)
{
    return ((Specular_D(a, NdH) * Specular_G(a, NdV, NdL, NdH, VdH, LdV)) * Specular_F(specularColor, v, h)) / (4.0f * NdL * NdV + 0.0001f);

}
float DistributionGGX(float dotNH, float roughness)
{
    float alpha = roughness;
    float alphaSqr = alpha * alpha;
    float denom = dotNH * dotNH * (alphaSqr - 1.0f) + 1.0f;
    
    float PI = PI_MACRO;
    
    return alphaSqr / (PI * denom * denom);
}
float3 FresnelSchlick(float cosTheta, float3 f0)
{
    return f0 + (1.0f - f0) * pow(1.0f - cosTheta, 5.0f);
}
float GeometrySchlickGGX(float NdotV, float k)
{
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}
float GeometrySmith(float3 N, float3 V, float3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1 * ggx2;
}


float3 EvaluateAmbience(TextureCube cubeMap, SamplerState defaultSampler, float3 vN, float3 org_normal, float3 to_cam, float perceptualRoughness, float metalness, float3 albedo, float ao, float3 dfcol, float3 spccol, float4 ambientStr)
{
    int numMips = GetNumMips(cubeMap);
    const int nrBrdfMips = numMips - nMipOffset;
    float VdotN = clamp(dot(to_cam, vN), 0.0, 1.0f);
    const float3 vRorg = 2 * vN * VdotN - to_cam;
    
    float3 vR = GetSpecularDominantDir(vN, vRorg, RoughnessFromPerceptualRoughness(perceptualRoughness));
    float RdotNsat = saturate(dot(vN, vR));
    
    float l = BurleyToMip(perceptualRoughness, numMips, RdotNsat);
    
    // Take the maxinum of Mips to get the Average color of skybox
    float3 specRad = cubeMap.SampleLevel(defaultSampler, vR, numMips).rgb;
    float3 diffRad = cubeMap.SampleLevel(defaultSampler, vN, (float) (nrBrdfMips - 1)).rgb;
    
    ambientStr.rgb *= ambientStr.w;
    if (length(specRad) == 0.f && length(diffRad) == 0.f)
    {
        diffRad = ambientStr.rgb;
        specRad = ambientStr.rgb; 
    }
    else
    {
        diffRad *= ambientStr.rgb;
        specRad *= ambientStr.rgb;
    }
    
    float fT = 1.0 - RdotNsat;
    float fT5 = fT * fT;
    fT5 = fT5 * fT5 * fT;
    spccol = lerp(spccol, (float) 1.0, fT5);
    
    float fFade = GetReductionInMicroFacets(perceptualRoughness);
    fFade *= EmpiricalSpecularAO(ao, perceptualRoughness);
    fFade *= ApproximateSpecularSelfOcclusion(vR, org_normal);
    
    float3 ambientdiffuse = ao * dfcol * diffRad;
    float3 ambientspecular = fFade * spccol * specRad;
    return ambientdiffuse + ambientspecular;
}

float3 EvaluateDirectionalLight(float3 albedoColor, float3 specularColor, float3 normal, float roughness, float3 lightColor, float3 lightDir, float3 viewDir)
{
    float PI = PI_MACRO;
    float NdL = saturate(dot(normal, lightDir));
    float lambert = NdL;
    float3 h = normalize(viewDir + lightDir);
    float NdH = saturate(dot(normal, h));
    
    float cosTheta = dot(lightDir, normal);
    
    float  D = DistributionGGX(NdH, roughness);
    float  G = GeometrySmith(normal, viewDir, lightDir, roughness);
    float3 F = FresnelSchlick(cosTheta, specularColor);
  
    float3 specular = ((D * G * F) / 4.f * dot(normal, lightDir)) * dot(normal, viewDir);
    float3 diffuse = max(dot(normal, lightDir), 0.0f) * albedoColor;
    diffuse *= 1.f / PI;
    
    return lightColor * lambert * (diffuse * (1.0f - specular) + specular) * PI;
}