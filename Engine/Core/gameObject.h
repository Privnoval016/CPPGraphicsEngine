//
// Created by Graphics Engine
//

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Components/component.h"
#include "Components/behaviour.h"
#include "Components/transformComponent.h"
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <typeindex>
#include <unordered_map>

class GameObject
{
public:
    std::string name;
    TransformComponent transform;
    bool active;

    GameObject(const std::string& objectName = "GameObject")
        : name(objectName),
          transform(),
          active(true)
    {
    }

    ~GameObject()
    {
        for (auto& component : components)
        {
            // Only call onDestroy on Behaviour components
            if (auto* behaviour = dynamic_cast<Behaviour*>(component.get())) {
                behaviour->onDestroy();
            }
        }
    }

    // Component management
    template<typename T>
    T* addComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
        
        auto component = std::make_shared<T>();
        component->gameObject = this;
        components.push_back(component);
        
        std::type_index typeIdx(typeid(T));
        componentMap[typeIdx] = component;
        
        return static_cast<T*>(component.get());
    }

    template<typename T>
    T* getComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
        
        std::type_index typeIdx(typeid(T));
        auto it = componentMap.find(typeIdx);
        
        if (it != componentMap.end())
        {
            return static_cast<T*>(it->second.get());
        }
        
        return nullptr;
    }

    template<typename T>
    const T* getComponent() const
    {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
        
        std::type_index typeIdx(typeid(T));
        auto it = componentMap.find(typeIdx);
        
        if (it != componentMap.end())
        {
            return static_cast<const T*>(it->second.get());
        }
        
        return nullptr;
    }

    template<typename T>
    bool hasComponent()
    {
        return getComponent<T>() != nullptr;
    }

    template<typename T>
    void removeComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
        
        std::type_index typeIdx(typeid(T));
        auto it = componentMap.find(typeIdx);
        
        if (it != componentMap.end())
        {
            // Only call onDestroy if it's a Behaviour
            if (auto* behaviour = dynamic_cast<Behaviour*>(it->second.get())) {
                behaviour->onDestroy();
            }
            componentMap.erase(it);
            
            components.erase(
                std::remove_if(components.begin(), components.end(),
                    [&](const std::shared_ptr<Component>& c) {
                        return c.get() == it->second.get();
                    }),
                components.end()
            );
        }
    }

    // Lifecycle methods
    void awake()
    {
        for (auto& component : components)
        {
            if (auto* behaviour = dynamic_cast<Behaviour*>(component.get())) {
                if (behaviour->enabled)
                    behaviour->awake();
            }
        }
    }

    void start()
    {
        for (auto& component : components)
        {
            if (auto* behaviour = dynamic_cast<Behaviour*>(component.get())) {
                if (behaviour->enabled)
                    behaviour->start();
            }
        }
    }

    void update(float deltaTime)
    {
        if (!active) return;
        
        for (auto& component : components)
        {
            if (auto* behaviour = dynamic_cast<Behaviour*>(component.get())) {
                if (behaviour->enabled)
                    behaviour->update(deltaTime);
            }
        }
    }

    void lateUpdate(float deltaTime)
    {
        if (!active) return;
        
        for (auto& component : components)
        {
            if (auto* behaviour = dynamic_cast<Behaviour*>(component.get())) {
                if (behaviour->enabled)
                    behaviour->lateUpdate(deltaTime);
            }
        }
    }

    void setActive(bool value)
    {
        active = value;
    }

    bool isActive() const
    {
        return active;
    }

private:
    std::vector<std::shared_ptr<Component>> components;
    std::unordered_map<std::type_index, std::shared_ptr<Component>> componentMap;
};

#endif //GAMEOBJECT_H
