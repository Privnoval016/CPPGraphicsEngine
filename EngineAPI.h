//
// Engine API - Header for user-written scripts
// Use this in MonoBehaviour scripts, NOT in main.cpp
//

#ifndef ENGINE_API_H
#define ENGINE_API_H

// Core components
#include "Engine/Core/Components/component.h"
#include "Engine/Core/Components/behaviour.h"
#include "Engine/Core/Components/monoBehaviour.h"
#include "Engine/Core/Components/transformComponent.h"
#include "Engine/Core/Components/cameraComponent.h"
#include "Engine/Core/Components/meshFilter.h"
#include "Engine/Core/Components/meshRenderer.h"
#include "Engine/Core/gameObject.h"
#include "Engine/Core/Systems/input.h"

// Math
#include "Engine/Math/vec2.h"
#include "Engine/Math/vec3.h"
#include "Engine/Math/mat4.h"

// Rendering
#include "Engine/Rendering/color.h"
#include "Engine/Rendering/Primitives/mesh.h"
#include "Engine/Rendering/camera.h"
#include "Engine/Rendering/light.h"
#include "Engine/Rendering/texture.h"
#include "Engine/Rendering/Materials/material.h"

// NOTE: This header does NOT include:
// - Scene class (use gameObject to interact with scene)
// - Engine::run() / Engine::runOpenGL() (only for main.cpp)
// - GameEngine class (only for main.cpp)
// - Loaders and serializers (only for setup code)

#endif // ENGINE_API_H
