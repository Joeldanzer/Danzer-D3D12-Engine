#include "stdafx.h"
#include "DebugRenderingData.h"

DebugRenderingData::DebugRenderingData(ID3D12Device* device)
{
	m_vertexAABBBuffer.Initialize(device, sizeof(DebugAABBData));
	m_vertexLineBuffer.Initialize(device, sizeof(DebugLineData));
}
DebugRenderingData::~DebugRenderingData(){}

void DebugRenderingData::RenderAABB(const Mat4f& transform, const Vect3f& m_center, const Vect3f& m_extents, const Vect4f& color)
{
	Vect4f globalCenter = Mul(transform, m_center);

	Mat4f newTransform = Mat4f::Identity;
	newTransform      *= Mat4f::CreateScale(m_extents);
	newTransform.Translation(globalCenter);

	DebugAABBData data = {
		 color,
		 newTransform.Transpose()
	};

	m_aabbRenderList.emplace_back(data);
}

void DebugRenderingData::RenderLine(const Vect3f& start, const Vect3f& end, const Vect4f& color)
{
	DebugLineData line{
		color,
		start,
		end
	};
	m_lineRenderList.emplace_back(line);
}

void DebugRenderingData::UpdateInstancesForRendering(const uint8_t frameIndex)
{
	if(m_aabbRenderList.size() > 0)
		m_vertexAABBBuffer.UpdateBuffer(reinterpret_cast<uint16_t*>(&m_aabbRenderList[0]), (uint32_t)m_aabbRenderList.size(), frameIndex);
	if(m_lineRenderList.size() > 0)
		m_vertexLineBuffer.UpdateBuffer(reinterpret_cast<uint16_t*>(&m_lineRenderList[0]), (uint32_t)m_lineRenderList.size(), frameIndex);
	//ClearInstances();
}

void DebugRenderingData::ClearInstances()
{
	m_aabbRenderList.clear();
	m_lineRenderList.clear();
	m_sphereRenderList.clear();
}
