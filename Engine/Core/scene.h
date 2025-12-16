#ifndef SCENE_H
#define SCENE_H

#include "gameObject.h"
#include "../Rendering/camera.h"
#include "../Rendering/light.h"
#include "../Rendering/Core/framebuffer.h"
#include "../Rendering/Core/rasterizer.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>

// Forward declaration
class GameEngine;

// Forward declare Engine namespace functions for friend access
namespace Engine {
    inline void run(Scene& scene, int targetFPS);
    inline void runOpenGL(Scene& scene, int width, int height, const std::string& title, int targetFPS);
}

/**
 * @class Scene
 * @brief Represents a 3D scene containing GameObjects, lights, and a camera
 * 
 * The Scene class manages a collection of GameObjects, lights, and the main camera.
 * It provides methods for lifecycle management (awake, start, update) and rendering.
 */
class Scene
{
    friend class GameEngine;  // Allow GameEngine to call private lifecycle methods
    friend void Engine::run(Scene&, int);
    friend void Engine::runOpenGL(Scene&, int, int, const std::string&, int);
public:
    std::string name;
    Camera mainCamera;
    std::vector<Light> lights;
    color backgroundColor;

    Scene(const std::string& sceneName = "Untitled Scene")
        : name(sceneName),
          mainCamera(),
          backgroundColor(0.1f, 0.1f, 0.15f)
    {
    }
    
    // OpenGL initialization callback
    // Set this callback to initialize materials and other OpenGL resources
    // It will be called automatically after OpenGL context is created
    void onOpenGLReady(std::function<void(Scene&)> callback)
    {
        openGLReadyCallback = callback;
    }

    /**
     * @brief Create and add a new GameObject to the scene
     * @param name Name of the GameObject
     * @return Pointer to the created GameObject
     */
    GameObject* createGameObject(const std::string& name = "GameObject")
    {
        auto obj = std::make_shared<GameObject>(name);
        gameObjects.push_back(obj);
        return obj.get();
    }

    void destroyGameObject(GameObject* obj)
    {
        gameObjects.erase(
            std::remove_if(gameObjects.begin(), gameObjects.end(),
                [obj](const std::shared_ptr<GameObject>& go) {
                    return go.get() == obj;
                }),
            gameObjects.end()
        );
    }

    // Light management
    void addLight(const Light& light)
    {
        lights.push_back(light);
    }

    void clearLights()
    {
        lights.clear();
    }

    // Rendering
    void render(Framebuffer& framebuffer, Rasterizer& rasterizer)
    {
        framebuffer.clear(backgroundColor);

        // Note: Software rasterizer rendering is deprecated
        // Use OpenGL renderer instead (Engine::runOpenGL)
        // This method kept for backwards compatibility
    }

    std::vector<GameObject*> getAllGameObjects() const
    {
        std::vector<GameObject*> result;
        result.reserve(gameObjects.size());
        for (auto& obj : gameObjects)
        {
            result.push_back(obj.get());
        }
        return result;
    }

    GameObject* findGameObject(const std::string& objectName)
    {
        for (auto& obj : gameObjects)
        {
            if (obj->name == objectName)
                return obj.get();
        }
        return nullptr;
    }

private:
    std::vector<std::shared_ptr<GameObject>> gameObjects;
    std::function<void(Scene&)> openGLReadyCallback;

    // Private lifecycle methods - only GameEngine should call these
    
    /**
     * @brief Invoke the OpenGL ready callback
     * Called by GameEngine after OpenGL context is created.
     */
    void _invokeOpenGLReady()
    {
        if (openGLReadyCallback)
        {
            openGLReadyCallback(*this);
        }
    }
    
    void awake()
    {
        for (auto& obj : gameObjects)
        {
            obj->awake();
        }
    }

    void start()
    {
        for (auto& obj : gameObjects)
        {
            obj->start();
        }
    }

    void update(float deltaTime)
    {
        for (auto& obj : gameObjects)
        {
            obj->update(deltaTime);
        }
    }

    void lateUpdate(float deltaTime)
    {
        for (auto& obj : gameObjects)
        {
            obj->lateUpdate(deltaTime);
        }
    }
};

#endif //SCENE_H
