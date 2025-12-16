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

// Forward declaration
class Scene;

/**
 * @class GameObject
 * @brief Represents an object in the scene with components
 * 
 * GameObject is the fundamental entity in the scene graph. It can have multiple
 * components attached to it, including (a required) TransformComponent for position/rotation/scale,
 * and Behaviour-derived components for custom logic.
 */
class GameObject
{
    friend class Scene;  // Allow Scene to call private lifecycle methods
public:
    std::string name;
    TransformComponent transform;
    bool active;

    /**
     * @brief Constructor for GameObject
     * @param objectName Name of the GameObject
     */
    GameObject(const std::string& objectName = "GameObject")
        : name(objectName),
          transform(),
          active(true)
    {
    }

    /**
     * @brief Destructor for GameObject
     * Calls onDestroy on all Behaviour components
     */
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

    /**
     * @brief Add a component of type T to the GameObject
     * @tparam T Type of the component to add (must derive from Component)
     * @return Pointer to the added component
     */
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

    /**
     * @brief Get a component of type T from the GameObject
     * @tparam T Type of the component to get (must derive from Component)
     * @return Pointer to the component if found, nullptr otherwise
     */
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

    /**
     * @brief Get a const component of type T from the GameObject
     * @tparam T Type of the component to get (must derive from Component)
     * @return Pointer to the component if found, nullptr otherwise
     */
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

    /**
     * @brief Check if the GameObject has a component of type T
     * @tparam T Type of the component to check (must derive from Component)
     * @return True if the component exists, false otherwise
     */
    template<typename T>
    bool hasComponent()
    {
        return getComponent<T>() != nullptr;
    }

    /**
     * @brief Remove a component of type T from the GameObject
     * @tparam T Type of the component to remove (must derive from Component)
     */
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

    // Private lifecycle methods - only Scene should call these
    
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
};

#endif //GAMEOBJECT_H
