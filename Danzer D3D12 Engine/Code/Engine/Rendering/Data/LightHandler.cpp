#include "stdafx.h"
#include "LightHandler.h"

#include "Core/D3D12Framework.h"

#include "Components/Light/PointLight.h"
#include "Components/Light/SpotLight.h"
#include "Components/Transform.h"

LightHandler::LightHandler(D3D12Framework& framework) :
    m_pointLightInstances({})
{
    m_spotLightBuffer.Initialize(framework.GetDevice(), sizeof(SpotLightData));
    m_pointLightBuffer.Initialize(framework.GetDevice(), sizeof(PointLightData));
}
LightHandler::~LightHandler(){}
void LightHandler::AddLightInstanceForRendering(const PointLight& light, const Transform& transform)
{
    m_pointLightInstances.push_back({
        transform.m_position + light.m_offset,
        light.m_color,
        light.m_range
    });
}
void LightHandler::AddLightInstanceForRendering(const SpotLight& light, Transform& transform){
    m_spotLightInstances.push_back({
        transform.m_position + light.m_offset,
        light.m_color,
        Vect4f(transform.World().Forward()),
        light.m_cutOff,
        light.m_outerCutOff,
        light.m_range
        });
}
void LightHandler::UpdateLightInstances(const uint8_t frameIndex)
{
    if(!m_pointLightInstances.empty())
        m_pointLightBuffer.UpdateBuffer(reinterpret_cast<uint16_t*>(&m_pointLightInstances[0]), (uint32_t)m_pointLightInstances.size(), frameIndex);
    
    if(!m_spotLightInstances.empty())
        m_spotLightBuffer.UpdateBuffer(reinterpret_cast<uint16_t*>(&m_spotLightInstances[0]), (uint32_t)m_spotLightInstances.size(), frameIndex);
}
void LightHandler::ClearAllInstances()
{
    m_pointLightInstances.clear();
    m_spotLightInstances.clear();
}

