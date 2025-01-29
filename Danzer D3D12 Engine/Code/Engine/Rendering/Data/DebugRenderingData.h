#pragma once
#include "Rendering/Buffers/VertexBuffer.h"

#define MAX_VISIBLE_AABB MAX_INSTANCES_PER_MODEL

struct ID3D12Device;

class DebugRenderingData
{
public:
	DebugRenderingData(ID3D12Device* device);
	~DebugRenderingData();

	// Data used for the vertex buffer.
	struct DebugAABBData {
		Vect4f m_color	   = {0.0f, 1.0f, 0.0f, 1.0f};
		Mat4f  m_transform = Mat4f::Identity;
	};

	struct DebugLineData {
		Vect4f m_color    = { 1.0f, 0.0f, 0.0f, 1.0f };
		Vect4f m_start    = Vect4f::Zero;
		Vect4f m_end      = Vect4f::Zero;
	};

	struct DebugSphereData {
		Vect4f m_color    = { 0.0f, 1.0f, 0.0f, 1.0f };
		Vect4f m_position = Vect4f::Zero;
		float  m_radius   = 1.0f;
	};

	// Uniform function so that this can be used for anything like the physics or basic debugging.
	void RenderAABB(const Mat4f& tranform, const Vect3f& m_center, const Vect3f& m_extents, const Vect4f& color = {0.0f, 1.0f, 0.f, 1.0f});
	void RenderAABB(DebugAABBData data) {
		m_aabbRenderList.emplace_back(data);
	}

	void RenderLine(const Vect3f& start, const Vect3f& end, const Vect4f& color = { 1.0f, 0.0f, 0.0f, 1.0f });
	void RenderLine(DebugLineData data) {
		m_lineRenderList.emplace_back(data);
	}

	void UpdateInstancesForRendering(const uint8_t frameIndex);

private:	
	friend class DebugTextureRenderer;

	// Clear Instances each frame. 
	void ClearInstances();

	std::vector<DebugSphereData> m_sphereRenderList;
	std::vector<DebugLineData>   m_lineRenderList;
	std::vector<DebugAABBData>   m_aabbRenderList;

	VertexBuffer				 m_vertexAABBBuffer;
	VertexBuffer				 m_vertexLineBuffer;
};

