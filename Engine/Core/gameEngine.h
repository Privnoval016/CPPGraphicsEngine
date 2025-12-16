#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "scene.h"
#include "Systems/input.h"
#include "../Rendering/Core/framebuffer.h"
#include "../Rendering/Core/rasterizer.h"
#include "../Rendering/Core/window.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

/**
 * @class GameEngine
 * @brief Main game engine class that manages the game loop, scenes, and rendering
 * 
 * The GameEngine class is responsible for initializing the engine, managing the active scene,
 * running the game loop, handling window events, and rendering frames. It supports both
 * fixed timestep and interactive rendering modes.
 */
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

    /**
     * @brief Constructor for GameEngine
     * @param w Width of the framebuffer/window
     * @param h Height of the framebuffer/window
     * @param createWindow Whether to create an SDL window for interactive rendering
     */
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

    /**
     * @brief Set the active scene for the engine
     * @param scene Pointer to the Scene to set as active
     */
    void setActiveScene(Scene* scene)
    {
        activeScene = scene;
        if (activeScene)
        {
            activeScene->mainCamera.setAspectRatio((float)width / (float)height);
        }
    }

    /**
     * @brief Initialize the engine and active scene
     * This calls the awake and start methods of the active scene
     */
    void initialize()
    {
        if (activeScene)
        {
            activeScene->awake();
            activeScene->start();
        }
    }

    /**
     * @brief Run a single frame of the engine
     * This updates the active scene and renders a frame
     */
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

    /**
     * @brief Run the engine for a fixed number of frames with a fixed timestep
     * @param numFrames Number of frames to run
     * @param fixedDeltaTime Fixed delta time for each frame
     */
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

    /**
     * @brief Run the engine in interactive mode with a window
     * This starts the main loop, handling window events and rendering frames
     */
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

    /**
     * @brief Stop the engine's main loop
     */
    void stop()
    {
        running = false;
    }

    /**
     * @brief Save the current framebuffer to a PPM file
     * @param filename Name of the file to save the framebuffer
     */
    void saveFrame(const std::string& filename)
    {
        framebuffer.saveToPPM(filename);
    }

    /**
     * @brief Output the current framebuffer to the console
     */
    void outputFrame()
    {
        framebuffer.outputToConsole();
    }

    /**
     * @brief Resize the framebuffer and update the active scene's camera aspect ratio
     * @param newWidth New width of the framebuffer/window
     * @param newHeight New height of the framebuffer/window
     */
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
