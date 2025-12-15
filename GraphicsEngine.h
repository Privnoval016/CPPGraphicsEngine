//
// Graphics Engine - Single header for users
// Include this to use the engine
//

#ifndef GRAPHICS_ENGINE_H
#define GRAPHICS_ENGINE_H

// Core components
#include "Engine/Core/Components/component.h"
#include "Engine/Core/Components/behaviour.h"
#include "Engine/Core/Components/monoBehaviour.h"
#include "Engine/Core/Components/transformComponent.h"
#include "Engine/Core/Components/cameraComponent.h"
#include "Engine/Core/gameObject.h"
#include "Engine/Core/scene.h"
#include "Engine/Core/Systems/sceneSerializer.h"
#include "Engine/Core/gameEngine.h"
#include "Engine/Core/Systems/input.h"

// Math
#include "Engine/Math/vec3.h"
#include "Engine/Math/mat4.h"

// Rendering
#include "Engine/Rendering/color.h"
#include "Engine/Rendering/Primitives/mesh.h"
#include "Engine/Rendering/camera.h"
#include "Engine/Rendering/light.h"
#include "Engine/Rendering/framebuffer.h"
#include "Engine/Rendering/rasterizer.h"
#include "Engine/Rendering/window.h"
#include "Engine/Rendering/opengl_window.h"
#include "Engine/Rendering/opengl_renderer.h"
#include "Engine/Rendering/Shaders/shader.h"
#include "Engine/Rendering/Shaders/default_shaders.h"

/**
 * @namespace Engine
 * @brief Simplified engine API for users
 * 
 * Provides high-level functions to run the engine without manual setup.
 */
namespace Engine
{
    /**
     * @brief Run the engine with a scene
     * @param scene Scene to run
     * @param targetFPS Target frames per second (0 = unlimited)
     * 
     * This handles the entire engine loop:
     * - Initializes renderer
     * - Calls awake/start on all components
     * - Runs update loop
     * - Handles cleanup
     * 
     * Example:
     * @code
     * Scene scene;
     * auto player = scene.createGameObject("Player");
     * player->addComponent<PlayerController>();
     * 
     * Engine::run(scene);
     * @endcode
     */
    inline void run(Scene& scene, int targetFPS = 60)
    {
        GameEngine engine;
        // Note: GameEngine doesn't have setTargetFPS yet, handled internally
        engine.setActiveScene(&scene);
        engine.run();
    }

    /**
     * @brief Run with OpenGL hardware rendering
     * @param scene Scene to run
     * @param width Window width
     * @param height Window height
     * @param title Window title
     * @param targetFPS Target FPS (0 = unlimited)
     */
    inline void runOpenGL(Scene& scene, int width = 1280, int height = 720, 
                         const std::string& title = "Graphics Engine", int targetFPS = 60)
    {
        OpenGLWindow window(width, height, title);
        if (!window.isOpen) {
            std::cerr << "Failed to create OpenGL window!" << std::endl;
            return;
        }

        OpenGLRenderer renderer;
        if (!renderer.initialize()) {
            std::cerr << "Failed to initialize renderer!" << std::endl;
            return;
        }

        // Call awake/start on all components
        scene.awake();
        scene.start();

        // Main loop
        Uint32 lastTime = SDL_GetTicks();
        int frameCount = 0;
        float fpsTimer = 0.0f;

        while (window.isOpen) {
            Uint32 currentTime = SDL_GetTicks();
            float deltaTime = (currentTime - lastTime) / 1000.0f;
            lastTime = currentTime;

            // Limit frame rate
            if (targetFPS > 0) {
                float targetDelta = 1.0f / targetFPS;
                if (deltaTime < targetDelta) {
                    SDL_Delay((Uint32)((targetDelta - deltaTime) * 1000));
                    deltaTime = targetDelta;
                }
            }

            // Poll events FIRST (SDL needs this for keyboard state)
            if (!window.pollEvents()) break;

            // Update input AFTER polling events
            Input::getInstance().update();

            // Update scene
            scene.update(deltaTime);
            scene.lateUpdate(deltaTime);

            // Render
            renderer.clear(0.1f, 0.1f, 0.15f);

            // Find main camera
            Camera* camera = nullptr;
            for (auto* obj : scene.getAllGameObjects()) {
                if (auto* camComp = obj->getComponent<CameraComponent>()) {
                    camera = camComp->getCamera();
                    break;
                }
            }

            if (!camera) {
                // No camera found, skip rendering
                window.swapBuffers();
                continue;
            }

            // Collect lights (TODO: LightComponent)
            std::vector<Light> lights;
            lights.push_back(Light::directional(vec3(-1, -1, -1), color(1, 1, 1), 0.8f));

            // Render all mesh objects
            for (auto* obj : scene.getAllGameObjects()) {
                if (obj->getMesh()) {
                    mat4 transform = obj->transform.getModelMatrix();
                    if (camera) {
                        renderer.drawMesh(*obj->getMesh(), transform, *camera, lights);
                    }
                }
            }

            window.swapBuffers();

            // FPS counter
            frameCount++;
            fpsTimer += deltaTime;
            if (fpsTimer >= 1.0f) {
                float currentFPS = frameCount / fpsTimer;
                std::string newTitle = title + " (" + std::to_string((int)currentFPS) + " FPS)";
                window.setTitle(newTitle);
                frameCount = 0;
                fpsTimer = 0.0f;
            }
        }

        // Cleanup
        // Note: Scene cleanup is automatic via destructors
        renderer.cleanup();
    }
}

#endif //GRAPHICS_ENGINE_H
