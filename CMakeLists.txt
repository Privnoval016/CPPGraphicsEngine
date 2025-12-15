cmake_minimum_required(VERSION 3.30)
project(CPPGraphicsEngine)

set(CMAKE_CXX_STANDARD 20)

# Find SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Find OpenGL
if (APPLE)
    find_library(OPENGL_LIBRARY OpenGL)
    set(OPENGL_LIBRARIES ${OPENGL_LIBRARY})
else()
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    set(OPENGL_LIBRARIES ${OPENGL_LIBRARIES} GLEW::GLEW)
endif()

# Engine source files
set(ENGINE_MATH
    Engine/Math/vec3.h
    Engine/Math/mat4.h
)

set(ENGINE_CORE
    Engine/Core/Components/component.h
    Engine/Core/Components/behaviour.h
    Engine/Core/Components/monoBehaviour.h
    Engine/Core/Components/transformComponent.h
    Engine/Core/Components/cameraComponent.h
    Engine/Core/Components/meshFilter.h
    Engine/Core/Components/meshRenderer.h
    Engine/Core/Systems/input.h
    Engine/Core/Systems/sceneSerializer.h
    Engine/Core/gameObject.h
    Engine/Core/scene.h
    Engine/Core/gameEngine.h
)

set(ENGINE_RENDERING
    Engine/Rendering/color.h
    Engine/Rendering/Primitives/mesh.h
    Engine/Rendering/camera.h
    Engine/Rendering/framebuffer.h
    Engine/Rendering/rasterizer.h
    Engine/Rendering/light.h
    Engine/Rendering/window.h
    Engine/Rendering/opengl_window.h
    Engine/Rendering/opengl_renderer.h
    Engine/Rendering/Shaders/shader.h
    Engine/Rendering/Shaders/default_shaders.h
    Engine/Rendering/texture.h
    Engine/Rendering/material.h
    Engine/Rendering/builtin_materials.h
)

set(ENGINE_HEADERS
    ${ENGINE_MATH}
    ${ENGINE_CORE}
    ${ENGINE_RENDERING}
    GraphicsEngine.h
)

set(USER_SCRIPTS
    Assets/Scripts/rotator.h
    Assets/Scripts/cameraController.h
)

# Main executable
add_executable(Game
    main.cpp
    ${ENGINE_HEADERS}
    ${USER_SCRIPTS}
)

target_link_libraries(Game ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})

# Material demo executable
add_executable(MaterialDemo
    material_demo.cpp
    ${ENGINE_HEADERS}
    ${USER_SCRIPTS}
)

target_link_libraries(MaterialDemo ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})

# Set as default target
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Game)

# Print build info
message(STATUS "==========================================")
message(STATUS "CPP Graphics Engine")
message(STATUS "==========================================")
message(STATUS "Executable: Game")
message(STATUS "Entry Point: main.cpp")
message(STATUS "")
message(STATUS "Project Structure:")
message(STATUS "  Assets/Scripts/  - Your game scripts")
message(STATUS "  Assets/Scenes/   - Saved scenes")
message(STATUS "  Engine/          - Engine source (don't modify)")
message(STATUS "  main.cpp         - Entry point")
message(STATUS "==========================================")
