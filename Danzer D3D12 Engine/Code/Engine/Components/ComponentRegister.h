#pragma once

#include "Core/RegistryWrapper.h"

#include <map>
#include <functional>
#include <string>
#include <iostream>

// The base of a component, each new component needs to be registered through BaseComponent . 
struct BaseComponent {   
    BaseComponent() {}

    // Function override for displaying in editor.
    virtual void  DisplayInEditor(Entity entity){};  

private:
    friend class ImguiHandler;
    friend class EntryRegister;
    friend class SceneLoader;

    // Used for editor and scene loader to be able to emplace components without needing a reference to the component struct. 
    virtual void  EmplaceComp(const Entity entity) {}

    // Used by scene loader to save data to scene file. 
    virtual void ComponentToData(const Entity entity, char* outData) {}
    virtual void DataToComponent(const Entity entity, char* inData) {}

    // Override these functions so that SceneLoader can load components properly, check Model.cpp for example. 
    virtual void WriteComponentToFile(const Entity entity, std::fstream& file);
    virtual void LoadFileToComponent(const  Entity entity, std::fstream& file);

    virtual const uint16_t SizeOfData() { return UINT16_MAX; }
};

//namespace COMPONENT_REGISTRY {
class EntryRegister {
    std::map<std::string, BaseComponent*> m_cRegister;

public:
    static EntryRegister& GetInstance() {
        static EntryRegister s_instance;
        return s_instance;
    }

    template<class T>
    bool RegComponent(const std::string& name) {

        if (m_cRegister.find(name) == m_cRegister.end()) {
            m_cRegister.insert({ name,  new T()});
            return true;
        }
        return false;
    }

    // Function for displaying & changing components values through ImGui.
    void DisplayComponent(const Entity entity, const std::string componentName) {
        if (!ComponentIsRegistered(componentName))
            return;

        m_cRegister[componentName]->DisplayInEditor(entity);
    }
    // Function for ImGui to emplace components through just fetching the components name. 
    void EmplaceComponent(const Entity entity, const std::string componentName) {
        if (!ComponentIsRegistered(componentName))
            return;

        m_cRegister[componentName]->EmplaceComp(entity);
    }

    // Gets a pointer of the based component to fetch component data or to emplace a component. 
    BaseComponent* GetBaseComponent(const Entity entity, const std::string componentName) {
        if (!ComponentIsRegistered(componentName))
            return nullptr;

        return m_cRegister[componentName];
    }

    const std::map<std::string, BaseComponent*>& GetComponentRegister() {
        return m_cRegister;
    }

private:
    bool ComponentIsRegistered(const std::string componentName) {
        const auto it = m_cRegister.find(componentName);
        if (it == m_cRegister.end()) {
            std::cout << "Component '" << componentName << "' does not exist in EntryRegister!" << std::endl;
            return false;
        }

        return true;
    }
};

#undef  COMPONENT_ENTRY_REGISTER
#define COMPONENT_ENTRY_REGISTER \
    EntryRegister::GetInstance()

template<class T>                              
class ComponentAdder {                                
public:
    explicit ComponentAdder(const std::string name) {
        if(EntryRegister::GetInstance().RegComponent<T>(name))
            std::cout << name << " Component Registered..." << std::endl;
    }
};       

// Registers component to EntryRegister 
#define REGISTER_COMPONENT(NewComponent) \
    const ComponentAdder<NewComponent> s_##NewComponent##Adder(COMPONENT_NAME(NewComponent)); 

// * Override functions for all Components derived by BaseComponent and gives EntryRegister the means to 
// * emplace/remove/fetch Component data without needing a reference to the component struct
#define COMP_FUNC(NewComponent)			                              \
private:                                                              \
void EmplaceComp(const Entity entity) override {                      \
	REGISTRY->Emplace<NewComponent>(entity);	                      \
} const uint16_t SizeOfData() override {                              \
    return sizeof(NewComponent);                                      \
} void ComponentToData(const Entity entity, char* outData) override { \
    NewComponent& comp = REGISTRY->Get<NewComponent>(entity);         \
    uint16_t size = sizeof(NewComponent);                             \
    memcpy(&outData[0], &comp, size);                                 \
} void DataToComponent(const Entity entity, char* inData)  override { \
    NewComponent& comp = REGISTRY->Get<NewComponent>(entity);         \
    uint16_t size = sizeof(NewComponent);                             \
    memcpy(&comp, &inData[0], size);                                  \
}                                                                     \
public:

//#else
//#define REGISTER_COMPONENT(NewComponent) 
//#define COMP_FUNC(NewComponent)
//#endif
