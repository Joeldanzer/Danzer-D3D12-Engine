#pragma once
#include "Core/MathDefinitions.h"

#include <string>
//#include "FontBuffer.h"
#include <unordered_map>

struct ID3D12Device;

class Font {
public:
	struct Letter {
		Letter() : m_letter(' '), m_position(0.0f, 0.0f) {}
		~Letter() {}

		//Letter(const Letter&);
		Letter(char letter, Vect2f pos, Vect2f size) :
			m_letter(letter),
			m_width((UINT)size.x),
			m_height((UINT)size.y),
			m_position{pos.x, pos.y}			
		{}

		char m_letter;

		Vect2f m_position;

		UINT m_width  = 0;
		UINT m_height = 0;
	};
	
	struct Instance {
		Vect4f m_fontPosition;
		Vect2f m_fontSize;
		Vect2f m_sheetSize;
		Vect2f m_position;
		Vect2f m_size;
		Vect2f m_anchor;
		Vect4f m_color;
	};

	struct Data {
		std::unordered_map<char, Letter> m_letters;

		UINT m_width   = 0;
		UINT m_height  = 0;
		UINT m_texture = 0;
	};

	Font()  = delete;
	~Font() = default;

	explicit Font(const Font&) = default;
	explicit Font(const Data& data, ID3D12Device* device, std::string name = "") :
		m_ID(0),
		m_name(name),
		m_data(data)
	{
		//m_instancebuffer.Init(device);
	}

	void SetID(UINT id) { m_ID = id; }
	UINT GetID() { return m_ID; }

	const std::string Name() { return m_name; }

	Data& GetData() { return m_data; }

	void AddInstance(Instance inst) { m_instances.emplace_back(inst); }
	void ClearInstances() { m_instances.clear(); }

	std::vector<Instance>& GetInstances() { return m_instances; }

	void UpdateInstanceBuffer(UINT frameIndex) { 
		//m_instancebuffer.UpdateBuffer(reinterpret_cast<UINT8*>(&m_instances[0]), (UINT)m_instances.size(), frameIndex); }
	//FontBuffer& GetInstanceBuffer() { return m_instancebuffer; 
	}

private:
	std::string m_name;
	UINT m_ID;
	Data m_data;

	std::vector<Instance> m_instances;
	//FontBuffer m_instancebuffer;
};