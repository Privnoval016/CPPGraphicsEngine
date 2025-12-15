//
// Created by Graphics Engine
//

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

class Scene
{
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
    
    // Internal: Called by Engine after OpenGL initialization
    void _invokeOpenGLReady()
    {
        if (openGLReadyCallback)
        {
            openGLReadyCallback(*this);
        }
    }

    // GameObject management
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

    // Scene lifecycle
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
};

#endif //SCENE_H
