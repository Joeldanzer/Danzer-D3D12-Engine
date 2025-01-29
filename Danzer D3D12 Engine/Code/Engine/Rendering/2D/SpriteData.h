#pragma once

// Sprite instance buffer, havent changed the name of the file yet
//#include "SpriteInstanceBuffer.h"
#include "../../Core/MathDefinitions.h"

#include <array>
#include <string>


class SpriteData {
public:
	struct Instance {
		Vect2f m_sheetSize = { 0.f, 0.f };
		Vect2f m_position  = { 0.f, 0.f };
		Vect2f m_anchor	   = { 0.f, 0.f };
		Vect2f m_frameSize = { 1.f, 1.f };
		Vect2f m_uiSize    = { 1.f, 1.f };
	};

	struct Frame {
		Vect2f m_framePosition;
		unsigned int m_width = 0;
		unsigned int m_height = 0;
	};

	struct Sheet {
		std::vector<Frame> m_frames;
		unsigned int m_width  = 0;
		unsigned int m_height = 0;

		UINT m_texture = 0;
	};

	SpriteData() = delete;
	~SpriteData() = default;
	explicit SpriteData(const SpriteData&) = default;
	explicit SpriteData(const Sheet& sheet, ID3D12Device* device, std::string name = "") :
		m_ID(0),
		m_name(name),
		m_spriteSheet(sheet),
		m_renderSprite(true)
	{
		//m_instanceBuffer.Init(device);
	};


	void SetID(UINT id) { m_ID = id; }
	UINT GetID() { return m_ID; }

	Sheet& GetSheet() {return m_spriteSheet;}
	const std::string& Name() { return m_name; }

	// Need to redo!
	//SpriteInstanceBuffer& GetInstanceBuffer() { return m_instanceBuffer; }
	void UpdateInstanceBuffer(UINT frameIndex) { 
		if(!m_instances.empty()){}
			//m_instanceBuffer.UpdateBuffer(
			//	reinterpret_cast<UINT8*>(&m_instances[0]), (UINT)m_instances.size(), frameIndex); 
	}
	void AddInstance(SpriteData::Instance instance) { m_instances.emplace_back(instance); }
	void ClearInstances() { m_instances.clear(); }
	const std::vector<SpriteData::Instance> GetInstances() {return m_instances;}

private:
	Sheet m_spriteSheet;

	std::vector<SpriteData::Instance> m_instances;

	///SpriteInstanceBuffer m_instanceBuffer;

	std::string m_name;
	UINT m_ID;

	bool m_renderSprite;

};