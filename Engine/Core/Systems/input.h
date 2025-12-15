//
// Created by Graphics Engine
//

#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>
#include <unordered_map>

class Input
{
public:
    static Input& getInstance()
    {
        static Input instance;
        return instance;
    }

    void update()
    {
        // Copy current state to previous
        previousKeyState = currentKeyState;
        previousMouseState = currentMouseState;
        
        // SDL_PollEvent already pumps events, but call explicitly to be safe
        // (This is already called by window.pollEvents() before this function)
        // SDL_PumpEvents();  // Not needed - pollEvents does this
        
        // Get current keyboard state from SDL
        // This returns a pointer to SDL's internal state array
        int numKeys;
        const Uint8* sdlKeyState = SDL_GetKeyboardState(&numKeys);
        
        // Clear and rebuild current state from SDL
        currentKeyState.clear();
        for (int i = 0; i < numKeys; i++)
        {
            if (sdlKeyState[i])
            {
                currentKeyState[(SDL_Scancode)i] = true;
            }
        }
        
        // Get mouse state
        int x, y;
        Uint32 buttons = SDL_GetMouseState(&x, &y);
        mouseX = x;
        mouseY = y;
        currentMouseState[SDL_BUTTON_LEFT] = (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
        currentMouseState[SDL_BUTTON_RIGHT] = (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
        currentMouseState[SDL_BUTTON_MIDDLE] = (buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
        
        // Calculate mouse delta using relative mouse state (works correctly in relative mouse mode)
        int relX, relY;
        SDL_GetRelativeMouseState(&relX, &relY);
        mouseDeltaX = relX;
        mouseDeltaY = relY;
    }

    // Key queries
    bool getKey(SDL_Scancode key)
    {
        if (currentKeyState.find(key) == currentKeyState.end())
            currentKeyState[key] = false;
        return currentKeyState[key];
    }

    bool getKeyDown(SDL_Scancode key)
    {
        bool current = getKey(key);
        bool previous = (previousKeyState.find(key) != previousKeyState.end()) ? previousKeyState[key] : false;
        return current && !previous;
    }

    bool getKeyUp(SDL_Scancode key)
    {
        bool current = getKey(key);
        bool previous = (previousKeyState.find(key) != previousKeyState.end()) ? previousKeyState[key] : false;
        return !current && previous;
    }

    // Mouse queries
    bool getMouseButton(int button)
    {
        if (currentMouseState.find(button) == currentMouseState.end())
            return false;
        return currentMouseState[button];
    }

    bool getMouseButtonDown(int button)
    {
        bool current = getMouseButton(button);
        bool previous = (previousMouseState.find(button) != previousMouseState.end()) ? previousMouseState[button] : false;
        return current && !previous;
    }

    bool getMouseButtonUp(int button)
    {
        bool current = getMouseButton(button);
        bool previous = (previousMouseState.find(button) != previousMouseState.end()) ? previousMouseState[button] : false;
        return !current && previous;
    }

    int getMouseX() const { return mouseX; }
    int getMouseY() const { return mouseY; }
    int getMouseDeltaX() const { return mouseDeltaX; }
    int getMouseDeltaY() const { return mouseDeltaY; }

    // Convenience key codes
    static const SDL_Scancode W = SDL_SCANCODE_W;
    static const SDL_Scancode A = SDL_SCANCODE_A;
    static const SDL_Scancode S = SDL_SCANCODE_S;
    static const SDL_Scancode D = SDL_SCANCODE_D;
    static const SDL_Scancode SPACE = SDL_SCANCODE_SPACE;
    static const SDL_Scancode SHIFT = SDL_SCANCODE_LSHIFT;
    static const SDL_Scancode CTRL = SDL_SCANCODE_LCTRL;
    static const SDL_Scancode ESC = SDL_SCANCODE_ESCAPE;
    static const SDL_Scancode UP = SDL_SCANCODE_UP;
    static const SDL_Scancode DOWN = SDL_SCANCODE_DOWN;
    static const SDL_Scancode LEFT = SDL_SCANCODE_LEFT;
    static const SDL_Scancode RIGHT = SDL_SCANCODE_RIGHT;

private:
    Input() : mouseX(0), mouseY(0), lastMouseX(0), lastMouseY(0), mouseDeltaX(0), mouseDeltaY(0) {}
    
    std::unordered_map<SDL_Scancode, bool> currentKeyState;
    std::unordered_map<SDL_Scancode, bool> previousKeyState;
    std::unordered_map<int, bool> currentMouseState;
    std::unordered_map<int, bool> previousMouseState;
    
    int mouseX, mouseY;
    int lastMouseX, lastMouseY;
    int mouseDeltaX, mouseDeltaY;
};

#endif //INPUT_H
