//
// Created by Graphics Engine
//

#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "scene.h"
#include "Systems/input.h"
#include "../Rendering/framebuffer.h"
#include "../Rendering/rasterizer.h"
#include "../Rendering/window.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

class GameEngine
{
public:
    int width;
    int height;
    bool running;
    
    Scene* activeScene;
    Framebuffer framebuffer;
    Rasterizer rasterizer;

    // Time management
    float deltaTime;
    float time;
    int frameCount;

    // Window rendering
    std::unique_ptr<Window> window;
    bool useWindow;

    GameEngine(int w = 800, int h = 600, bool createWindow = false)
        : width(w),
          height(h),
          running(false),
          activeScene(nullptr),
          framebuffer(w, h),
          rasterizer(),
          deltaTime(0.0f),
          time(0.0f),
          frameCount(0),
          useWindow(createWindow)
    {
        if (useWindow)
        {
            window = std::make_unique<Window>(w, h, "CPP Graphics Engine");
            if (!window->isOpen)
            {
                std::cerr << "Failed to create window!" << std::endl;
                useWindow = false;
            }
        }
    }

    void setActiveScene(Scene* scene)
    {
        activeScene = scene;
        if (activeScene)
        {
            activeScene->mainCamera.setAspectRatio((float)width / (float)height);
        }
    }

    void initialize()
    {
        if (activeScene)
        {
            activeScene->awake();
            activeScene->start();
        }
    }

    void runFrame()
    {
        if (!activeScene || !running)
            return;

        // Update scene
        activeScene->update(deltaTime);
        activeScene->lateUpdate(deltaTime);

        // Render scene
        activeScene->render(framebuffer, rasterizer);

        frameCount++;
        time += deltaTime;
    }

    void run(int numFrames = 1, float fixedDeltaTime = 1.0f / 60.0f)
    {
        running = true;
        initialize();

        deltaTime = fixedDeltaTime;

        for (int i = 0; i < numFrames && running; i++)
        {
            runFrame();
        }
    }

    void runInteractive()
    {
        if (!activeScene)
        {
            std::cerr << "No active scene set!" << std::endl;
            return;
        }

        if (!useWindow || !window || !window->isOpen)
        {
            std::cerr << "Window not available! Use GameEngine(width, height, true) to enable window." << std::endl;
            return;
        }

        running = true;
        initialize();

        auto lastTime = std::chrono::high_resolution_clock::now();
        
        std::cout << "Starting interactive loop (Press ESC to exit)..." << std::endl;

        while (running && window->isOpen)
        {
            auto currentTime = std::chrono::high_resolution_clock::now();
            deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;

            // Cap deltaTime to prevent huge jumps
            if (deltaTime > 0.1f)
                deltaTime = 0.1f;

            // Poll window events
            if (!window->pollEvents())
            {
                running = false;
                break;
            }

            // Update input
            Input::getInstance().update();

            // Handle window resize
            if (window->width != width || window->height != height)
            {
                resize(window->width, window->height);
            }

            // Run frame
            runFrame();

            // Display to window
            if (useWindow && window->isOpen)
            {
                auto pixelData = framebuffer.getPixelData();
                window->display(pixelData.data());
            }

            // Update FPS in title
            if (frameCount % 30 == 0)
            {
                float fps = 1.0f / deltaTime;
                std::string title = "CPP Graphics Engine - FPS: " + std::to_string(static_cast<int>(fps)) + 
                                  " | Frame: " + std::to_string(frameCount);
                window->setTitle(title);
            }
        }

        std::cout << "\nInteractive loop ended." << std::endl;
        std::cout << "Total frames: " << frameCount << std::endl;
        std::cout << "Total time: " << time << "s" << std::endl;
        std::cout << "Average FPS: " << (frameCount / time) << std::endl;
    }

    void stop()
    {
        running = false;
    }

    void saveFrame(const std::string& filename)
    {
        framebuffer.saveToPPM(filename);
    }

    void outputFrame()
    {
        framebuffer.outputToConsole();
    }

    // Resize the framebuffer
    void resize(int newWidth, int newHeight)
    {
        width = newWidth;
        height = newHeight;
        framebuffer.resize(width, height);
        
        if (activeScene)
        {
            activeScene->mainCamera.setAspectRatio((float)width / (float)height);
        }
    }
};

#endif //GAME_ENGINE_H
