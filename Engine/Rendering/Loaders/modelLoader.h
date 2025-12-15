#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include "../Primitives/mesh.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>

/**
 * @class ModelLoader
 * @brief Loads 3D models from various file formats
 * 
 * Supported formats:
 * - OBJ (Wavefront .obj files)
 * 
 * Future support planned:
 * - FBX (Autodesk FBX files)
 */
class ModelLoader
{
private:
    // Face vertex data structure
    struct FaceVertex {
        int posIndex;
        int texIndex;
        int normIndex;
    };

public:
    /**
     * @brief Load model from file (auto-detects format)
     * @param filepath Path to model file
     * @param autoTriangulate If true, automatically triangulate and save non-triangle faces
     * @return Loaded mesh or nullptr if failed
     */
    static std::shared_ptr<Mesh> loadFromFile(const std::string& filepath, bool autoTriangulate = true)
    {
        // Detect file format from extension
        std::string extension = getFileExtension(filepath);
        
        if (extension == "obj") {
            return loadOBJ(filepath, autoTriangulate);
        }
        else if (extension == "fbx") {
            std::cerr << "FBX format not yet supported" << std::endl;
            return nullptr;
        }
        else {
            std::cerr << "Unsupported model format: " << extension << std::endl;
            return nullptr;
        }
    }

    /**
     * @brief Load Wavefront OBJ file
     * @param filepath Path to .obj file
     * @param autoTriangulate If true, automatically triangulate and save non-triangle faces
     * @return Loaded mesh or nullptr if failed
     */
    static std::shared_ptr<Mesh> loadOBJ(const std::string& filepath, bool autoTriangulate = true)
    {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to open OBJ file: " << filepath << std::endl;
            return nullptr;
        }

        std::vector<vec3> positions;
        std::vector<vec3> normals;
        std::vector<vec2> texCoords;
        
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        // Track if we need to triangulate and rewrite
        bool needsTriangulation = false;
        std::vector<std::string> fileLines;
        
        std::string line;
        while (std::getline(file, line)) {
            // Store original line for potential rewrite
            std::string originalLine = line;
            
            if (line.empty() || line[0] == '#') {
                fileLines.push_back(originalLine);
                continue;
            }
            
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;
            
            if (prefix == "v") {
                // Vertex position
                vec3 pos;
                iss >> pos.x >> pos.y >> pos.z;
                positions.push_back(pos);
                fileLines.push_back(originalLine);
            }
            else if (prefix == "vn") {
                // Vertex normal
                vec3 normal;
                iss >> normal.x >> normal.y >> normal.z;
                normals.push_back(normal);
                fileLines.push_back(originalLine);
            }
            else if (prefix == "vt") {
                // Texture coordinate
                float u, v;
                iss >> u >> v;
                texCoords.push_back(vec2(u, v));
                fileLines.push_back(originalLine);
            }
            else if (prefix == "f") {
                // Face (triangle or quad or n-gon)
                std::vector<FaceVertex> faceVertices;
                std::string vertexData;
                
                while (iss >> vertexData) {
                    FaceVertex fv = parseFaceVertex(vertexData);
                    faceVertices.push_back(fv);
                }
                
                // Check if face needs triangulation
                if (faceVertices.size() > 3) {
                    needsTriangulation = true;
                    
                    // Write triangulated faces
                    for (size_t i = 1; i < faceVertices.size() - 1; i++) {
                        std::ostringstream triangleLine;
                        triangleLine << "f";
                        
                        // Format: v/vt/vn or v//vn or v/vt or v
                        auto formatVertex = [](const FaceVertex& fv) -> std::string {
                            std::ostringstream ss;
                            ss << " " << (fv.posIndex + 1);  // OBJ is 1-based
                            if (fv.texIndex >= 0 || fv.normIndex >= 0) {
                                ss << "/";
                                if (fv.texIndex >= 0) ss << (fv.texIndex + 1);
                                if (fv.normIndex >= 0) {
                                    ss << "/" << (fv.normIndex + 1);
                                }
                            }
                            return ss.str();
                        };
                        
                        triangleLine << formatVertex(faceVertices[0]);
                        triangleLine << formatVertex(faceVertices[i]);
                        triangleLine << formatVertex(faceVertices[i + 1]);
                        
                        if (i == 1) {
                            // Replace original face line with first triangle
                            fileLines.push_back(triangleLine.str() + " # triangulated");
                        } else {
                            // Add additional triangles as new lines
                            fileLines.push_back(triangleLine.str());
                        }
                    }
                } else {
                    // Keep original triangle face
                    fileLines.push_back(originalLine);
                }
                
                // Triangulate face for loading (supports any n-gon with fan triangulation)
                if (faceVertices.size() >= 3) {
                    for (size_t i = 1; i < faceVertices.size() - 1; i++) {
                        addVertex(vertices, indices, faceVertices[0], positions, normals, texCoords);
                        addVertex(vertices, indices, faceVertices[i], positions, normals, texCoords);
                        addVertex(vertices, indices, faceVertices[i + 1], positions, normals, texCoords);
                    }
                }
            }
            else {
                // Keep other lines (mtllib, usemtl, etc.)
                fileLines.push_back(originalLine);
            }
        }
        
        file.close();
        
        // Auto-triangulate: rewrite file if needed
        if (needsTriangulation && autoTriangulate) {
            std::cout << "Model contains non-triangle faces. Auto-triangulating and updating file..." << std::endl;
            
            std::ofstream outFile(filepath);
            if (outFile.is_open()) {
                outFile << "# Auto-triangulated by CPP Graphics Engine\n";
                for (const auto& fileLine : fileLines) {
                    outFile << fileLine << "\n";
                }
                outFile.close();
                std::cout << "File updated with triangulated faces: " << filepath << std::endl;
            } else {
                std::cerr << "Warning: Could not update file with triangulated faces" << std::endl;
            }
        }
        
        if (vertices.empty()) {
            std::cerr << "No vertices found in OBJ file: " << filepath << std::endl;
            return nullptr;
        }
        
        // Calculate normals if not provided
        if (normals.empty()) {
            calculateNormals(vertices, indices);
        }
        
        auto mesh = std::make_shared<Mesh>();
        mesh->vertices = vertices;
        
        // Convert indices to triangles
        for (size_t i = 0; i < indices.size(); i += 3) {
            mesh->triangles.push_back(Triangle{(int)indices[i], (int)indices[i+1], (int)indices[i+2]});
        }
        
        std::cout << "Loaded OBJ: " << filepath << std::endl;
        std::cout << "  Vertices: " << vertices.size() << std::endl;
        std::cout << "  Triangles: " << indices.size() / 3 << std::endl;
        
        return mesh;
    }

private:
    static std::string getFileExtension(const std::string& filepath)
    {
        size_t dotPos = filepath.find_last_of('.');
        if (dotPos == std::string::npos) return "";
        
        std::string ext = filepath.substr(dotPos + 1);
        // Convert to lowercase
        for (char& c : ext) {
            c = std::tolower(c);
        }
        return ext;
    }

    static FaceVertex parseFaceVertex(const std::string& vertexData)
    {
        FaceVertex fv = {-1, -1, -1};
        
        std::istringstream iss(vertexData);
        std::string indexStr;
        int i = 0;
        
        while (std::getline(iss, indexStr, '/')) {
            if (!indexStr.empty()) {
                int index = std::stoi(indexStr);
                // OBJ indices are 1-based, convert to 0-based
                index = (index > 0) ? index - 1 : index;
                
                if (i == 0) fv.posIndex = index;
                else if (i == 1) fv.texIndex = index;
                else if (i == 2) fv.normIndex = index;
            }
            i++;
        }
        
        return fv;
    }

    static void addVertex(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                         const FaceVertex& fv, const std::vector<vec3>& positions,
                         const std::vector<vec3>& normals, const std::vector<vec2>& texCoords)
    {
        Vertex vertex;
        
        // Position
        if (fv.posIndex >= 0 && fv.posIndex < (int)positions.size()) {
            vertex.position = positions[fv.posIndex];
        }
        
        // Normal
        if (fv.normIndex >= 0 && fv.normIndex < (int)normals.size()) {
            vertex.normal = normals[fv.normIndex];
        } else {
            vertex.normal = vec3(0, 1, 0);  // Default up
        }
        
        // Texture coordinates
        if (fv.texIndex >= 0 && fv.texIndex < (int)texCoords.size()) {
            vertex.setTexCoord(texCoords[fv.texIndex]);
        } else {
            vertex.setTexCoord(vec2(0, 0));
        }
        
        // Default vertex color (white)
        vertex.vertexColor = color(1, 1, 1);
        
        indices.push_back(vertices.size());
        vertices.push_back(vertex);
    }

    static void calculateNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    {
        // Reset all normals to zero
        for (auto& v : vertices) {
            v.normal = vec3(0, 0, 0);
        }
        
        // Calculate face normals and accumulate
        for (size_t i = 0; i < indices.size(); i += 3) {
            unsigned int i0 = indices[i];
            unsigned int i1 = indices[i + 1];
            unsigned int i2 = indices[i + 2];
            
            vec3 v0 = vertices[i0].position;
            vec3 v1 = vertices[i1].position;
            vec3 v2 = vertices[i2].position;
            
            vec3 edge1 = v1 - v0;
            vec3 edge2 = v2 - v0;
            vec3 normal = vec3::cross(edge1,  edge2);
            
            vertices[i0].normal = vertices[i0].normal + normal;
            vertices[i1].normal = vertices[i1].normal + normal;
            vertices[i2].normal = vertices[i2].normal + normal;
        }
        
        // Normalize all normals
        for (auto& v : vertices) {
            float len = v.normal.length();
            if (len > 0) {
                v.normal = v.normal / len;
            }
        }
    }
};

#endif
