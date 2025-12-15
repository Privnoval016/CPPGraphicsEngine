//
// OpenGL Window - Hardware-accelerated window with OpenGL context
//

#ifndef OPENGL_WINDOW_H
#define OPENGL_WINDOW_H

#include <SDL2/SDL.h>
#include <string>
#include <iostream>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

class OpenGLWindow
{
public:
    SDL_Window* window;
    SDL_GLContext glContext;
    
    int width;
    int height;
    bool isOpen;

    OpenGLWindow(int w, int h, const std::string& title = "CPP Graphics Engine - OpenGL")
        : window(nullptr),
          glContext(nullptr),
          width(w),
          height(h),
          isOpen(false)
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
            return;
        }

        // Set OpenGL version
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        
#ifdef __APPLE__
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        window = SDL_CreateWindow(
            title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width,
            height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
        );

        if (!window)
        {
            std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return;
        }

        glContext = SDL_GL_CreateContext(window);
        if (!glContext)
        {
            std::cerr << "OpenGL context creation failed: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            SDL_Quit();
            return;
        }

        // Enable VSync
        SDL_GL_SetSwapInterval(1);

#ifndef __APPLE__
        // Initialize GLEW (not needed on macOS)
        glewExperimental = GL_TRUE;
        GLenum glewError = glewInit();
        if (glewError != GLEW_OK)
        {
            std::cerr << "GLEW initialization failed: " << glewGetErrorString(glewError) << std::endl;
            SDL_GL_DeleteContext(glContext);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return;
        }
#endif

        isOpen = true;
        
        // Set initial viewport
        glViewport(0, 0, width, height);
        
        std::cout << "OpenGL Window created: " << width << "x" << height << std::endl;
        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
        std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    }

    ~OpenGLWindow()
    {
        close();
    }

    void close()
    {
        if (glContext)
        {
            SDL_GL_DeleteContext(glContext);
            glContext = nullptr;
        }
        if (window)
        {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
        SDL_Quit();
        isOpen = false;
    }

    void swapBuffers()
    {
        if (!isOpen) return;
        SDL_GL_SwapWindow(window);
    }

    // Poll for window events
    bool pollEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                isOpen = false;
                return false;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    isOpen = false;
                    return false;
                }
            }
            else if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    width = event.window.data1;
                    height = event.window.data2;
                    glViewport(0, 0, width, height);
                }
            }
        }
        return true;
    }

    void setTitle(const std::string& title)
    {
        if (window)
            SDL_SetWindowTitle(window, title.c_str());
    }
};

#endif //OPENGL_WINDOW_H
