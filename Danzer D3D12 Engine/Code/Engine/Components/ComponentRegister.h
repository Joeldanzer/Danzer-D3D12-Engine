#pragma once

#include "Core/RegistryWrapper.h"

#include <map>
#include <functional>
#include <string>
#include <iostream>

// The base of a component, each new component needs to be registered through BaseComponent to be able to be seen in the Editor UI. 
struct BaseComponent {   
    BaseComponent() {}

    virtual void DisplayInEditor(Entity entity){};  // Function override for displaying in editor.
    virtual void EmplaceComp(const Entity entity){} // Used for editor to be able to emplace components to Editor

private:
    friend class ImguiHandler;
    friend class EntryRegister;
};

namespace COMPONENT_REGISTRY {
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
        void DisplayComponent(const Entity entity, const std::string name) {
            const auto it = m_cRegister.find(name);
            if (it == m_cRegister.end()) {
                std::cout << "Component '" << name << "' does not exist in EntryRegister!" << std::endl;
                return;
            }

            it->second->DisplayInEditor(entity);
        }
        // Function for ImGui to emplace components through just fetching the components name. 
        void EmplaceComponent(const Entity entity, const std::string name) {
            const auto it = m_cRegister.find(name);
            if (it == m_cRegister.end()) {
                std::cout << "Component '" << name << "' does not exist in EntryRegister!" << std::endl;
                return;
            }
            it->second->EmplaceComp(entity);
        }

        const std::map<std::string, BaseComponent*>& GetComponentRegister() {
            return m_cRegister;
        }
    };

    //#define REGISTER_COMPONENT(TYPE)                                                                                 \                                                                    \
//        template<class B, class T>                                                                               \
//        class ComponentRegistration;                                                                             \
//                                                                                                                 \
//        template<>                                                                                               \
//        class ComponentRegistration<BaseComponent, TYPE>                                                         \
//        {                                                                                                        \
//        public:                                                                                                  \
//            explicit ComponentRegistration(const char* name){                                                    \
//                ComponentFactory<BaseComponent>::GetInstance().RegisterComponentStruct<TYPE>(std::string(name)); \
//            }                                                                                                    \
//        };                                                                                                       \
//        ComponentRegistration<BaseComponent, TYPE> registerComponent(typeid(TYPE).name());                       \                                                                              \
//                                                                                                                 
//       // const ::component::detail::RegistryEntry<TYPE>&                       \
//       //     ComponentRegistration<TYPE>::reg =                                \
//       //         ::component::detail::RegistryEntry<TYPE>::Instance(NAME);     \
    
    template<class T>                              
    class ComponentAdder {                                
    public:
        explicit ComponentAdder(const std::string name) {
            if(EntryRegister::GetInstance().RegComponent<T>(name))
                std::cout << name << " Component Registered..." << std::endl;
        }
    };       
}

#undef  COMPONENT_ENTRY_REGISTER
#define COMPONENT_ENTRY_REGISTER \
    COMPONENT_REGISTRY::EntryRegister::GetInstance()

#ifdef EDITOR_DEBUG_VIEW
// Registers component to EntryRegister 
#define REGISTER_COMPONENT(NewComponent) \
    const COMPONENT_REGISTRY::ComponentAdder<NewComponent> s_##NewComponent##Adder(COMPONENT_NAME(NewComponent)); 
// Overrides EmplaceComp function in base component and allows the editor to automatically emplace components on to entities.
#define COMP_FUNC(NewComponent)			         \
void EmplaceComp(const Entity entity) override { \
	REGISTRY->Emplace<NewComponent>(entity);	 \
}
#else
#define REGISTER_COMPONENT(NewComponent) 
#define COMP_FUNC(NewComponent)
#endif
