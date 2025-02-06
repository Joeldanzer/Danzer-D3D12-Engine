#pragma once

struct ComponentData {
	uint8_t*    m_data;
	uint32_t    sizeOfData;
	std::string m_type;
};

// The base of a component, each new component needs to be registered to be able to be seen in the Editor UI. 
struct BaseComponent {
	const std::string& ComponentName() const {
		return m_componentName;
	}

protected:
	void DisplayComponentData() {}; // Used to 
	const std::string m_componentName = typeid(this).name();

private:
	friend class ImguiHandler;
};
