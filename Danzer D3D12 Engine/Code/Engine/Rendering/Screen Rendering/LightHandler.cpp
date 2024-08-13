#include "stdafx.h"
#include "LightHandler.h"

#include "Core/D3D12Framework.h"

LightHandler::LightHandler(D3D12Framework& framework) :
    m_framework(framework)
{}
LightHandler::~LightHandler(){}

UINT LightHandler::CreatePointLight()
{
    m_pointLightBuffer.emplace_back(PointLightBuffer());
    UINT id = (UINT)m_pointLightBuffer.size() - 1;
    m_pointLightBuffer[id].Init(m_framework.GetDevice(), &m_framework.CbvSrvHeap(), m_pointLightBuffer[id].FetchData(), sizeof(PointLightBuffer::Data));
    return (UINT)m_pointLightBuffer.size() - 1;
}
