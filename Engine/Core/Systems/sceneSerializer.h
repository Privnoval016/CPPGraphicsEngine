//
// Scene Serialization - Save and load scenes to/from files
//

#ifndef ENGINE_SCENE_SERIALIZER_H
#define ENGINE_SCENE_SERIALIZER_H

#include "../scene.h"
#include "../Components/meshFilter.h"
#include "../Components/meshRenderer.h"
#include <fstream>
#include <sstream>
#include <iostream>

// TODO : Finish serialization so it supports loading the actual monobehaviours

/**
 * @class SceneSerializer
 * @brief Handles saving and loading scenes to/from files
 * 
 * Simple text-based format for scene serialization.
 * Format:
 * - GameObject lines: GO <name> <active>
 * - Transform lines: TR <posX> <posY> <posZ> <rotX> <rotY> <rotZ> <scaleX> <scaleY> <scaleZ>
 * - Mesh lines: MESH <type> [params]
 */
class SceneSerializer
{
public:
    /**
     * @brief Save scene to file
     * @param scene Scene to save
     * @param filepath Path to save file
     * @return true if successful
     */
    static bool saveScene(const Scene& scene, const std::string& filepath)
    {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing: " << filepath << std::endl;
            return false;
        }

        file << "SCENE " << scene.name << "\n";
        file << "BGCOLOR " << scene.backgroundColor.x << " " 
             << scene.backgroundColor.y << " " << scene.backgroundColor.z << "\n";

        // Save all game objects
        for (const auto* obj : scene.getAllGameObjects()) {
            file << "GO " << obj->name << " " << (obj->isActive() ? "1" : "0") << "\n";
            
            // Save transform
            const auto& t = obj->transform;
            vec3 pos = t.getWorldPosition();
            vec3 rot = t.getWorldRotation();
            vec3 scl = t.getWorldScale();
            file << "TR " 
                 << pos.x << " " << pos.y << " " << pos.z << " "
                 << rot.x << " " << rot.y << " " << rot.z << " "
                 << scl.x << " " << scl.y << " " << scl.z << "\n";

            // Save mesh if MeshFilter component exists
            auto meshFilter = obj->getComponent<MeshFilter>();
            if (meshFilter && meshFilter->hasMesh()) {
                // For now, just save primitive type info
                // In a full implementation, you'd save vertex/triangle data
                file << "MESH primitive\n";
            }

            file << "ENDGO\n";
        }

        file.close();
        std::cout << "Scene saved to: " << filepath << std::endl;
        return true;
    }

    /**
     * @brief Load scene from file
     * @param filepath Path to scene file
     * @return Loaded scene (or empty scene if failed)
     */
    static Scene loadScene(const std::string& filepath)
    {
        Scene scene;
        std::ifstream file(filepath);
        
        if (!file.is_open()) {
            std::cerr << "Failed to open file for reading: " << filepath << std::endl;
            return scene;
        }

        std::string line;
        GameObject* currentObject = nullptr;

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string token;
            iss >> token;

            if (token == "SCENE") {
                std::string sceneName;
                std::getline(iss, sceneName);
                scene.name = sceneName.empty() ? sceneName : sceneName.substr(1); // Remove leading space
            }
            else if (token == "BGCOLOR") {
                float r, g, b;
                iss >> r >> g >> b;
                scene.backgroundColor = color(r, g, b);
            }
            else if (token == "GO") {
                std::string name;
                int active;
                iss >> name >> active;
                currentObject = scene.createGameObject(name);
                currentObject->setActive(active != 0);
            }
            else if (token == "TR" && currentObject) {
                float px, py, pz, rx, ry, rz, sx, sy, sz;
                iss >> px >> py >> pz >> rx >> ry >> rz >> sx >> sy >> sz;
                currentObject->transform.setPosition(vec3(px, py, pz));
                currentObject->transform.setRotation(vec3(rx, ry, rz));
                currentObject->transform.setScale(vec3(sx, sy, sz));
            }
            else if (token == "MESH" && currentObject) {
                std::string meshType;
                iss >> meshType;
                // For now, create a default cube with MeshFilter/MeshRenderer
                // In full implementation, load actual mesh data
                currentObject->addComponent<MeshFilter>()->setMesh(Mesh::createCube());
                currentObject->addComponent<MeshRenderer>();
            }
            else if (token == "ENDGO") {
                currentObject = nullptr;
            }
        }

        file.close();
        std::cout << "Scene loaded from: " << filepath << std::endl;
        return scene;
    }

    /**
     * @brief Quick save scene to Assets/Scenes/
     * @param scene Scene to save
     * @param name Scene name (without extension)
     */
    static bool saveSceneToAssets(const Scene& scene, const std::string& name)
    {
        std::string filepath = "Assets/Scenes/" + name + ".scene";
        return saveScene(scene, filepath);
    }

    /**
     * @brief Quick load scene from Assets/Scenes/
     * @param name Scene name (without extension)
     */
    static Scene loadSceneFromAssets(const std::string& name)
    {
        std::string filepath = "Assets/Scenes/" + name + ".scene";
        return loadScene(filepath);
    }
};

#endif //ENGINE_SCENE_SERIALIZER_H
