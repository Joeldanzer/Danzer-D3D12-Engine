#pragma once

#include "Core/RegistryWrapper.h"

#include <map>
#include <functional>
#include <string>
#include <iostream>

// The base of a component, each new component needs to be registered through BaseComponent to be able to be seen in the Editor UI. 
struct BaseComponent {    
    const std::string& cName() { return typeid(this).name(); } // Components name. 
    
#ifdef EDITOR_DEBUG_VIEW
    //static BaseComponent* StaticComponent(){
    //    if (s_staticComponent == nullptr)
    //        s_staticComponent = new BaseComponent();
    //
    //    //static BaseComponent* component;
    //    return s_staticComponent;
    //}
#endif    
    BaseComponent() {}
    const std::string& ComponentName() const {
        return m_componentName;
    }

    virtual void DisplayInEditor(Entity entity){}; // Function override for displaying in editor.

private:
    friend class ImguiHandler;
    friend class EntryRegister;

    std::string m_componentName;

    //static BaseComponent* s_staticComponent;
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
                //m_cRegister.insert({ name, []() -> BaseComponent* {return T::DisplayInEditor(Entity); }});
                m_cRegister.insert({ name,  new T()});
                return true;
            }
            return false;
        }

        void DisplayComponent(Entity entity, const std::string name) {
            const auto it = m_cRegister.find(name);
            if (it == m_cRegister.end()) {
                std::cout << "Component '" << name << "' does not exist in EntryRegister!" << std::endl;
                return;
            }

            it->second->DisplayInEditor(entity);
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
                std::cout << name << " Registered." << std::endl;
        }
    };                                                            
}

#undef  COMPONENT_ENTRY_REGISTER
#define COMPONENT_ENTRY_REGISTER \
    COMPONENT_REGISTRY::EntryRegister::GetInstance()

// Registers component to EntryRegister 
#ifdef EDITOR_DEBUG_VIEW
#define REGISTER_COMPONENT(NewComponent) \
    const COMPONENT_REGISTRY::ComponentAdder<NewComponent> s_##NewComponent##Adder(COMPONENT_NAME(NewComponent)); 
#else
#define REGISTER_COMPONENT(NewComponent) 
#endif